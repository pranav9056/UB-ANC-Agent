#ifndef UBNETWORK_H
#define UBNETWORK_H

#include <QObject>
#include <QByteArray>

class QTcpSocket;

class UBNetwork : public QObject
{
    Q_OBJECT
public:
    explicit UBNetwork(QObject *parent = 0);

private:

signals:
    void dataReady(quint32 srcID, QByteArray data);

public slots:
    void startNetwork(quint8 id, quint16 port);
    void sendData(quint32 desID, const QByteArray& data);

protected slots:
    void dataReadyEvent();

private:
    quint32 m_id;

    QTcpSocket* m_socket;
    QByteArray m_data;
};

#endif // UBNETWORK_H
