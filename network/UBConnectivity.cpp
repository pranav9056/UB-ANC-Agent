#include "QsLog.h"
#include "UBConnectivity.h"
#include "UBNetwork.h"
#include "UBAgent.h"
#include "location.h"
#include "graph.h"
#include "ArduPilotMegaMAV.h"
#include "config.h"
#include <QSet>
#include <QMutableHashIterator>

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
        QLOG_DEBUG()<<neighbors.keys().count()<<"::"<<neighbors.keys()<<"::"<<agent->m_uav->getUASID();
    }
}
void UBConnectivity::collectionPhase(int idx) {
    current.setLocation(agent->m_uav->getLatitude(),agent->m_uav->getLongitude());
    const Waypoint* temp = agent->m_mission_data.wpm->getWaypoint(idx);
    target.setLocation(temp->getLatitude(),temp->getLongitude());
    QLOG_INFO()<<agent->m_uav->getUASID()<<"::"<<"START:: "<<current.lat<<" "<<current.lon<<"TARGERT:: "<<target.lat<<" "<<target.lon<<endl;
    //neighbors.clear();
    emit broadcastLocation(BROADCAST_ID,current.serialize());

}

void UBConnectivity::proposalPhase(int idx){
    int noVertices = neighbors.keys().count() + 1;
    QLOG_DEBUG()<<agent->m_uav->getUASID()<<"::"<<"vertices "<<noVertices;
    Graph gTopo(noVertices,neighbors.keys(),agent->m_uav->getUASID());
    QSet<quint32> includedVertices;
    QMutableHashIterator<quint32,Location> vert(neighbors);
    while(vert.hasNext()){
        vert.next();
        QLOG_DEBUG()<<vert.key()<<"--"<<Location::distance(vert.value(),current);
        QLOG_FATAL()<<"before adding";
        gTopo.addEdge(agent->m_uav->getUASID(),vert.key(),(int)Location::distance(current,vert.value()));
        QLOG_FATAL()<<"after";

        includedVertices.insert(vert.key());
        QMutableHashIterator<quint32,Location> vert2(neighbors);
        while(vert2.hasNext()){
            QLOG_FATAL()<<"hereaa";
            vert2.next();
            if (includedVertices.contains(vert2.key()))
                continue;
            double dist = Location::distance(vert.value(),vert2.value());
            if (dist<COMM_RANGE)
                gTopo.addEdge(vert.key(),vert2.key(),(int)dist);
        }
    }
    QSet<int> filter = gTopo.primMST(agent->m_uav->getUASID());
    QLOG_INFO()<<agent->m_uav->getUASID()<<" filtered "<<filter.values();


}

void UBConnectivity::adjustmentPhase() {

}
