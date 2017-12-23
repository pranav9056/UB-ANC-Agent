#ifndef UBCONNECTIVITY_H
#define UBCONNECTIVITY_H

#include <QObject>
#include "UBAgent.h"
#include "location.h"
#include <QHash>
#include <QSet>
class UBNetwork;
class UBAgent;
class UBConnectivity : public QObject
{
    Q_OBJECT
public:
    explicit UBConnectivity(UBAgent *agent);

signals:
    void broadcastLocation(quint32 srcID, QByteArray data);
public slots:
    void collectionPhase(int idx);
    void getNeighbors(quint32,QByteArray);
    void proposalPhase(int idx);
    void getProposals(quint32,QByteArray);
    void adjustmentPhase(int idx);
    Location findProposal();
private:
protected:
    UBNetwork* m_net;
    UBAgent* agent;
    Location current;
    Location target;
    Location Tproposal;
    QHash<quint32,Location> neighbors;
    QHash<quint32,Location> proposals;
    QSet<int> filter;
};

#endif // UBCONNECTIVITY_H
