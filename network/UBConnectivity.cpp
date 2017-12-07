#include "QsLog.h"
#include "UBConnectivity.h"
#include "UBNetwork.h"
#include "UBAgent.h"
#include "location.h"
#include "ArduPilotMegaMAV.h"
#include "config.h"

UBConnectivity::UBConnectivity(UBAgent* agent)
{
    this->agent = agent;
    //connect(agent->m_net,SIGNAL(dataReady(quint32, QByteArray)), this, SLOT(getNeighbors(quint32,QByteArray)));


}

void UBConnectivity::getNeighbors(quint32 id, QByteArray data){
    if(data.size()==23 && data.startsWith("B:L")){
        Location temp;
        temp.deserialize(data);
        QLOG_INFO()<<"srcid::"<<id<<" data::"<<temp.lat<<"::"<<temp.lon<<"::"<<agent->m_uav->getUASID();
        neighbors.insert(id,temp);
        QLOG_DEBUG()<<neighbors.keys()<<"::"<<agent->m_uav->getUASID();
    }
}
void UBConnectivity::collectionPhase(int idx) {
    current.setLocation(agent->m_uav->getLatitude(),agent->m_uav->getLongitude());
    const Waypoint* temp = agent->m_mission_data.wpm->getWaypoint(idx);
    target.setLocation(temp->getLatitude(),temp->getLongitude());
    QLOG_INFO()<<"START:: "<<current.lat<<" "<<current.lon<<"TARGERT:: "<<target.lat<<" "<<target.lon<<endl;
    neighbors.clear();
    emit broadcastLocation(BROADCAST_ID,current.serialize());

}

void UBConnectivity::proposalPhase() {

}

void UBConnectivity::adjustmentPhase() {

}
