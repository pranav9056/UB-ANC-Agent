#include "UBAgent.h"

#include "config.h"
#include "QsLog.h"
#include <QThread>
#include "UBNetwork.h"
#include "UBVision.h"
#include "UBPower.h"
#include "UBConnectivity.h"

#include "UASManager.h"
#include "LinkManager.h"
#include "ArduPilotMegaMAV.h"
#include "LinkManagerFactory.h"
#include "mercatorprojection.h"
UBAgent::UBAgent(QObject *parent) : QObject(parent),
    m_uav(NULL),
    m_mission_stage(STAGE_IDLE)
{
    m_net = new UBNetwork(this);
    m_power = new UBPower(this);
    m_sensor = new UBVision(this);
    connect(m_net, SIGNAL(dataReady(quint32, QByteArray)), this, SLOT(dataReadyEvent(quint32, QByteArray)));

    m_timer = new QTimer(this);
    m_timer->setInterval(MISSION_TRACK_RATE);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(missionTracker()));

    //connectivity thread
    m_connectivity = new UBConnectivity(this);
    m_connectivity->moveToThread(&connectivityThread);
    connect(&connectivityThread, &QThread::finished, m_connectivity, &QObject::deleteLater);
    connect(this, SIGNAL(goToNextPoint(int)), m_connectivity,SLOT(collectionPhase(int)));
    connect(this,SIGNAL(proposalBegin(int)),m_connectivity,SLOT(proposalPhase(int)));
    //connect(worker, &Worker::resultReady, this, &Controller::handleResults);

    connect(m_connectivity,SIGNAL(broadcastLocation(quint32,QByteArray)),m_net,SLOT(sendData(quint32,QByteArray))); // To broadcast location
    connect(m_net,SIGNAL(dataReady(quint32,QByteArray)), m_connectivity, SLOT(getNeighbors(quint32,QByteArray)),Qt::QueuedConnection);
    connectivityThread.start();

}


void UBAgent::startAgent() {
    int port = MAV_PORT;
    getWayPointList();
    int idx = QCoreApplication::arguments().indexOf("--port");
    if (idx > 0)
        port = QCoreApplication::arguments().at(idx + 1).toInt();

    int link = 0;

    // Setup TCP if running the Emulator or Serial for running on hardware
    if(RUN_EMULATOR){
        link = LinkManagerFactory::addTcpConnection(QHostAddress::LocalHost, "", port, false);
    }
    else{
        link = LinkManagerFactory::addSerialConnection(SERIAL_PORT, BAUD_RATE);
    }

    LinkManager::instance()->connectLink(link);
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(UASCreatedEvent(UASInterface*)));
    //m_connectivity->start();
}

void UBAgent::setDestination(double lat,double lon){
    Waypoint wp;
    wp.setFrame(MAV_FRAME_GLOBAL_RELATIVE_ALT);
    wp.setLatitude(lat);
    wp.setLongitude(lon);
    wp.setAltitude(m_uav->getAltitudeRelative());
    m_uav->getWaypointManager()->goToWaypoint(&wp);
}

void UBAgent::getWayPointList() {
    m_mission_data.wpm = new UASWaypointManager();
    m_mission_data.wpm->loadWaypoints("mission.txt");
    return;
}


void UBAgent::UASCreatedEvent(UASInterface* uav) {
    if (m_uav)
        return;

    m_uav = qobject_cast<ArduPilotMegaMAV*>(uav);

    if (!m_uav)
        return;

    m_uav->setHeartbeatEnabled(true);

    connect(m_uav, SIGNAL(armed()), this, SLOT(armedEvent()));
    connect(m_uav, SIGNAL(disarmed()), this, SLOT(disarmedEvent()));
    connect(m_uav, SIGNAL(navModeChanged(int,int,QString)), this, SLOT(navModeChangedEvent(int,int)));

    int port = MAV_PORT;

    int idx = QCoreApplication::arguments().indexOf("--port");
    if (idx > 0)
        port = QCoreApplication::arguments().at(idx + 1).toInt();

    m_net->startNetwork(m_uav->getUASID(), (PHY_PORT - MAV_PORT) + port);
    m_power->startPWRSensor((PWR_PORT - MAV_PORT) + port);
    m_sensor->startSensor((SNR_PORT - MAV_PORT) + port);

//    QTimer::singleShot(START_DELAY, m_uav, SLOT(armSystem()));
    m_timer->start();
}

void UBAgent::armedEvent() {
//    if (m_uav->getGroundSpeed() > 1)
//        return;

//    if (!inPointZone(m_uav->getLatitude(), m_uav->getLongitude(), 0))
//        return;

    if (m_uav->getSatelliteCount() < GPS_ACCURACY)
        return;

    if (m_uav->getCustomMode() != ApmCopter::GUIDED)
//        m_uav->setMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, ApmCopter::GUIDED);
        return;

    m_power->sendData(UBPower::PWR_START, QByteArray());

//    m_uav->executeCommand(MAV_CMD_MISSION_START, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    m_uav->executeCommand(MAV_CMD_NAV_TAKEOFF, 1, 0, 0, 0, 0, 0, 0, TAKEOFF_ALT, 0);
    m_net->sendData(m_uav->getUASID() + 1, QByteArray(1, MAV_CMD_NAV_TAKEOFF));

    m_mission_stage = STAGE_TAKEOFF;
}

void UBAgent::disarmedEvent() {
    m_mission_stage = STAGE_IDLE;
}

void UBAgent::navModeChangedEvent(int uasID, int mode) {
    if (uasID != m_uav->getUASID())
        return;

//    if (mode == ApmCopter::GUIDED)
//        return;

    if (m_mission_stage != STAGE_IDLE) {
        m_mission_stage = STAGE_IDLE;
        QLOG_WARN() << "Mission Interrupted!";
    }
}

void UBAgent::dataReadyEvent(quint32 srcID, QByteArray data) {
    if(srcID == m_uav->getUASID() - 1)
        if (!m_uav->isArmed())
            m_uav->armSystem();
    //emit neigh(srcID,data);
}

double UBAgent::distance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2) {
   double x1, y1, z1;
   double x2, y2, z2;

   projections::MercatorProjection proj;

   proj.FromGeodeticToCartesian(lat1, lon1, alt1, x1, y1, z1);
   proj.FromGeodeticToCartesian(lat2, lon2, alt2, x2, y2, z2);

   return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

bool UBAgent::inPointZone(double lat, double lon, double alt) {
    double dist = distance(m_uav->getLatitude(), m_uav->getLongitude(), m_uav->getAltitudeRelative(), lat, lon, alt);

    if (dist < POINT_ZONE)
        return true;

    return false;
}

void UBAgent::missionTracker() {
//    if (m_start_time < START_DELAY) {
//        m_start_time++;
//        return;
//    }

    switch (m_mission_stage) {
    case STAGE_IDLE:
        stageIdle();
        break;
    case STAGE_TAKEOFF:
        stageTakeoff();
        break;
    case STAGE_MISSION:
        stageMission();
        break;
    case STAGE_LAND:
        stageLand();
        break;
    default:
        break;
    }
}

void UBAgent::stageIdle() {
//    m_uav->getWaypointManager()->readWaypoints(true);
//    m_net->sendData(BROADCAST_ADDRESS, QByteArray(1, BROADCAST_ADDRESS) + QByteArray(1, m_uav->getUASID()));
}

void UBAgent::stageTakeoff() {
    if (inPointZone(m_uav->getLatitude(), m_uav->getLongitude(), TAKEOFF_ALT)) {
        m_mission_data.reset();
        m_mission_stage = STAGE_MISSION;
        QLOG_INFO() << "Mission Begin";
    }
}

void UBAgent::stageLand() {
//    m_uav->setMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, ApmCopter::RTL);
    m_power->sendData(UBPower::PWR_STOP, QByteArray());

    m_uav->land();

    m_mission_stage = STAGE_IDLE;
    QLOG_INFO() << "Mission End";
}

void UBAgent::stageMission() {
    //static double lat, lon;
    static const Waypoint* temp;
    int idx = m_mission_data.wpm->getWaypointEditableList().size();
    static int nextPoint = 1;
    static int wait = 0;
    static int nextPointFlag = 1;
    if(m_mission_data.stage<idx){
        if(nextPoint)
        {
            if (nextPointFlag){
                emit(goToNextPoint(m_mission_data.stage));
                nextPointFlag = 0;
            }
            if(wait<5){
                wait++;
                return;
            }
            //QThread::currentThread()->msleep(10000);
            //QLOG_FATAL()<<"after sleep "<<m_uav->getUASID();
            emit(proposalBegin(m_mission_data.stage));
            QThread::currentThread()->msleep(1500);
            temp = m_mission_data.wpm->getWaypoint(m_mission_data.stage);
            setDestination(temp->getLatitude(),temp->getLongitude());
            nextPoint=0;
            nextPointFlag = 1;
            wait = 0;
        }
        else{
            if(inPointZone(temp->getLatitude(),temp->getLongitude(),m_uav->getAltitudeRelative())){
                m_mission_data.stage+=1;
                nextPoint=1;
            }
        }
        return;
    }
//    if (m_mission_data.stage == 0) {
//        m_mission_data.stage++;

//        projections::MercatorProjection proj;

//        double res = proj.GetGroundResolution(GND_RES, m_uav->getLatitude());
//        core::Point pix = proj.FromLatLngToPixel(m_uav->getLatitude(), m_uav->getLongitude(), GND_RES);
//        internals::PointLatLng pll = proj.FromPixelToLatLng(pix.X() + 10 / res, pix.Y(), GND_RES);

//        lat = pll.Lat();
//        lon = pll.Lng();

//        Waypoint wp;
//        wp.setFrame(MAV_FRAME_GLOBAL_RELATIVE_ALT);
// //        wp.setFrame(MAV_FRAME_GLOBAL_LOCAL_NED);
// //        wp.setAction(MAV_CMD_NAV_WAYPOINT);
// //        wp.setParam1(0);
// //        wp.setParam2(POINT_ZONE);
// //        wp.setParam3(POINT_ZONE);
// //        wp.setParam4(0);
// //        wp.setParam5(m_uav->getLatitude());
// //        wp.setParam6(m_uav->getLongitude());
// //        wp.setParam7(TAKEOFF_ALT);
// //        wp.setParam5(m_uav->getLocalX() + 10);
// //        wp.setParam6(m_uav->getLocalY());
// //        wp.setParam7(m_uav->getLocalZ());
//        wp.setLatitude(lat);
//        wp.setLongitude(lon);
//        wp.setAltitude(m_uav->getAltitudeRelative());

//        m_uav->getWaypointManager()->goToWaypoint(&wp);

//        return;
//    }

//    if (m_mission_data.stage == 1) {
//        if (inPointZone(lat, lon, m_uav->getAltitudeRelative())) {
//            m_mission_data.stage++;
//        }

//        return;
//    }

    if (m_mission_data.tick < 20) {
        m_mission_data.tick++;
    } else {
        m_mission_stage = STAGE_LAND;
    }
}
