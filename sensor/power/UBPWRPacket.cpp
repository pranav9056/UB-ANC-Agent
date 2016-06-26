#include "UBPWRPacket.h"

UBPWRPacket::UBPWRPacket(QObject *parent) : QObject(parent),
    m_cmd(0)
{

}

QByteArray UBPWRPacket::packetize(void) {
    QByteArray cmd(((char*)(&m_cmd)), sizeof(quint8));

    QByteArray payload(m_payload);

    return cmd + payload;
}

void UBPWRPacket::depacketize(const QByteArray& packet) {
    m_cmd = *((quint32*)(packet.mid(0, sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint32) + sizeof(quint32));
}
