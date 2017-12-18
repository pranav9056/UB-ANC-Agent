#ifndef UBCONNECTIVITY_H
#define UBCONNECTIVITY_H

#include <QObject>
#include "UBAgent.h"
#include "location.h"
#include <QHash>
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
    void adjustmentPhase();
private:
protected:
    UBNetwork* m_net;
    UBAgent* agent;
    Location current;
    Location target;
    QHash<quint32,Location> neighbors;
};

#endif // UBCONNECTIVITY_H
