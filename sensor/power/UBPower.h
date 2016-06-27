#ifndef UBPOWER_H
#define UBPOWER_H

#include <QObject>
#include <QByteArray>

class QTcpSocket;

class UBPower : public QObject
{
    Q_OBJECT
public:
    explicit UBPower(QObject *parent = 0);

private:

public:
    enum {
        PWR_START,
        PWR_STOP,
        PWR_EVENT,
    };

signals:
    void dataReady(quint8 cmd, QByteArray data);

public slots:
    void startPWRSensor(quint16 port);
    void sendData(quint8 cmd, const QByteArray& data);

protected slots:
    void dataReadyEvent();

private:
    QTcpSocket* m_socket;
    QByteArray m_data;
};

#endif // UBPOWER_H
