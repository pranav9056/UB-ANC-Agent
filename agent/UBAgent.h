#ifndef UBAGENT_H
#define UBAGENT_H

#include <QObject>
#include<QThread>
class QTimer;
class QThread;
class UASInterface;
class ArduPilotMegaMAV;

class UBNetwork;
class UBVision;
class UBPower;
class UBConnectivity;
class UASWaypointManager;

class UBAgent : public QObject
{
    Q_OBJECT
    QThread connectivityThread;
public:
    friend class UBConnectivity;
    explicit UBAgent(QObject *parent = 0);
    ~UBAgent(){
        connectivityThread.quit();
        connectivityThread.wait();
    }

public slots:
    void startAgent();

private:
    enum EMissionStage {
        STAGE_IDLE,
        STAGE_TAKEOFF,
        STAGE_MISSION,
        STAGE_LAND,
    } m_mission_stage;

    struct SMissionData {
        int stage;
        int tick;
        UASWaypointManager * wpm;
        //UASWaypointManager * wpm;
        void reset() {
            stage = 0;
            tick = 0;
        }
    } m_mission_data;

private:
    void stageIdle();
    void stageTakeoff();
    void stageMission();
    void stageLand();

signals:
    void goToNextPoint(int idx);
    void neigh(quint32 srcID, QByteArray data);

protected slots:
    void armedEvent();
    void disarmedEvent();

    void UASCreatedEvent(UASInterface *uav);
    void navModeChangedEvent(int uasID, int mode);

    void dataReadyEvent(quint32 srcID, QByteArray data);

    void missionTracker();
    void setDestination(double lat, double lon);
    void getWayPointList();


protected:
    QTimer* m_timer;
    ArduPilotMegaMAV* m_uav;
    UBNetwork* m_net;
    UBVision* m_sensor;
    UBPower* m_power;
    UBConnectivity* m_connectivity;

protected:
    double distance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2);
    bool inPointZone(double lat, double lon, double alt);
};

#endif // UBAGENT_H
