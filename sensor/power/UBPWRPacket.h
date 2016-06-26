#ifndef UBPWRPACKET_H
#define UBPWRPACKET_H

#include <QObject>

class UBPWRPacket : public QObject
{
    Q_OBJECT
public:
    explicit UBPWRPacket(QObject *parent = 0);

signals:

public slots:
    void setCMD(quint32 cmd) {m_cmd = cmd;}
    void setPayload(const QByteArray& payload) {m_payload = payload;}

private:
    quint8 m_cmd;

    QByteArray m_payload;

public:
    quint32 getCMD(void) {return m_cmd;}
    QByteArray getPayload(void) {return m_payload;}

    QByteArray packetize(void);
    void depacketize(const QByteArray &packet);
};

#endif // UBPWRPACKET_H
