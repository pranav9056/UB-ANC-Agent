#ifndef UBCONNECTIVITY_H
#define UBCONNECTIVITY_H

#include <QObject>
#include <QThread>
class UBNetwork;
class UBAgent;
class UBConnectivity : public QThread
{
    Q_OBJECT
public:
    explicit UBConnectivity(UBAgent *agent);
    void run();

signals:

public slots:
    void collectionPhase();
    void proposalPhase();
    void adjustmentPhase();
private:
protected:
    UBNetwork* m_net;
    UBAgent* agent;
};

#endif // UBCONNECTIVITY_H
