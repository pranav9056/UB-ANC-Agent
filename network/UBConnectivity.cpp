#include "QsLog.h"
#include "UBConnectivity.h"
#include "UBNetwork.h"
#include "UBAgent.h"
#include "location.h"
#include "graph.h"
#include "circle.h"
#include "ArduPilotMegaMAV.h"
#include "config.h"
#include "mercatorprojection.h"
#include <QSet>
#include <cmath>
#include <QMutableHashIterator>
#include "Waypoint.h"


double normD(core::Point a,core::Point b){
    return sqrt(pow(a.X()-b.X(),2)+pow(a.Y()-b.Y(),2));
}


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
    emit broadcastLocation(BROADCAST_ID,current.serialize("B:L"));

}


void UBConnectivity::getProposals(quint32 id,QByteArray data){
    if(data.size()==23 && data.startsWith("P:L")){
        Location temp;
        temp.deserialize(data);
        QLOG_INFO()<<"srcid::"<<id<<" data::"<<temp.lat<<"::"<<temp.lon<<"::"<<agent->m_uav->getUASID();
        proposals.insert(id,temp);
        QLOG_DEBUG()<<proposals.keys().count()<<"::"<<proposals.keys()<<"::"<<agent->m_uav->getUASID();
    }
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
    filter = gTopo.primMST(agent->m_uav->getUASID());
    QLOG_INFO()<<agent->m_uav->getUASID()<<" filtered "<<filter.values();
    // Find Proposal
    Location proposal = findProposal();
    emit broadcastLocation(BROADCAST_ID,proposal.serialize("P:L"));

    


}


Location UBConnectivity::findProposal(){
    QSet<int>::iterator i;
    QList<Circle> circles;
    for (i = filter.begin(); i != filter.end(); ++i)
    {
        Location temp = neighbors.value(*i);
        projections::MercatorProjection proj;
        core::Point pix1 = proj.FromLatLngToPixel(temp.lat, temp.lon, GND_RES);
        circles.append(Circle(COMM_RANGE,pix1));
    }
    projections::MercatorProjection proj;
    core::Point destination = proj.FromLatLngToPixel(target.lat,target.lon,GND_RES);
    core::Point proposal;
    if (circles.size()==1){
        Circle temp = circles.at(0);
        if(temp.checkPoint(destination)){
            proposal = destination;
        }else{
            proposal = temp.closestCirclePointFromPoint(destination);
        }
    }
    else if (circles.size()==2){
        core::Point i1;
        core::Point i2;
        Circle temp = circles.at(0);
        Circle temp1 = circles.at(1);
        if(temp.checkPoint(destination) && temp1.checkPoint(destination)){
            proposal = destination;
        }
        else{
            size_t no = temp.intersect(temp1,i1,i2);
            if (no==1){
                proposal =i1;
            }
            else{
                if(normD(destination,i1)<normD(destination,i2))
                    proposal = i1;
                else
                    proposal = i2;
            }
        }
    }
    else{
        int noC = circles.size();
        QList<core::Point> pboundary;
        QList<core::Point> boundary;
        int checkDest  = 1;
        for(int i = 0 ;i<noC-1;i++){
            Circle temp = circles.at(i);
            if(!temp.checkPoint(destination))
                checkDest = 0;
            for(int j = i+1;i<noC;j++){
                Circle temp1 = circles.at(j);
                core::Point i1;
                core::Point i2;
                size_t no = temp.intersect(temp1,i1,i2);
                pboundary.append(i1);
                if(no==2){
                    pboundary.append(i2);
                }
            }
        }

        QList<core::Point>::iterator i;
        double min=std::numeric_limits<double>::max();
        Circle last = circles.at(noC-1);
        if(checkDest == 1 && last.checkPoint(destination)){
            proposal=destination;
        }
        else{
            for (i = pboundary.begin(); i != pboundary.end(); ++i){
                for(int j = 0 ;j<noC;j++){
                    Circle temp = circles.at(j);
                    if (!temp.checkPoint(*i)){
                        break;
                    }
                    if (j==noC-1){
                        double dist = normD(*i,destination);
                        if(dist<min){
                            dist = min;
                            proposal.SetX((*i).X());
                            proposal.SetY((*i).Y());
                        }
                        boundary.append(*i);
                    }
                }
            }
        }
    }
    internals::PointLatLng prop = proj.FromPixelToLatLng(proposal.X(),proposal.Y(),GND_RES);
    Tproposal.setLocation(prop.Lat(),prop.Lng());
    return Location(prop.Lat(),prop.Lng());
}

void UBConnectivity::adjustmentPhase(int idx) {
    QSet<int>::iterator i;
    projections::MercatorProjection proj;
    int flag=0;
    core::Point proposal = proj.FromLatLngToPixel(Tproposal.lat,Tproposal.lon,GND_RES);
    for (i = filter.begin(); i != filter.end(); ++i)
    {
        Location temp = proposals.value(*i);
        core::Point temp1 = proj.FromLatLngToPixel(temp.lat,temp.lon,GND_RES);
        if (normD(proposal,temp1)>COMM_RANGE){
            flag=1;
            break;
        }
    }
    if(flag){
        core::Point st = proj.FromLatLngToPixel(current.lat,current.lon,GND_RES);
        proposal.SetX((proposal.X()+st.X())/2);
        proposal.SetY((proposal.Y()+st.Y())/2);
    }
    internals::PointLatLng final = proj.FromPixelToLatLng(proposal.X(),proposal.Y(),GND_RES);

    Waypoint wp;
    wp.setFrame(MAV_FRAME_GLOBAL_RELATIVE_ALT);
    wp.setLatitude(final.Lat());
    wp.setLongitude(final.Lng());
    wp.setAltitude(agent->m_uav->getAltitudeRelative());
    this->agent->m_mission_data.wpm->addWaypointEditable(&wp);
    int seq = this->agent->m_mission_data.wpm->getWaypointEditableList().size();
    this->agent->m_mission_data.wpm->moveWaypoint(seq-1,idx);


}
