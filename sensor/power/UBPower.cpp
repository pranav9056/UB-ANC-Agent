#include "UBPower.h"
#include "UBPWRPacket.h"

#include "config.h"
#include "QsLog.h"

#include <QTcpSocket>
#include <QHostAddress>

UBPower::UBPower(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReadyEvent()));
}

void UBPower::startPWRSensor(quint16 port) {
    m_socket->connectToHost(QHostAddress::LocalHost, port);
}

void UBPower::sendData(quint8 cmd, const QByteArray& data) {
    UBPWRPacket packet;

    packet.setCMD(cmd);
    packet.setPayload(data);

    m_socket->write(packet.packetize().append(PACKET_END));
}

void UBPower::dataReadyEvent() {
    m_data += m_socket->readAll();

    while (m_data.contains(PACKET_END)) {
        int bytes = m_data.indexOf(PACKET_END);

        UBPWRPacket packet;
        packet.depacketize(m_data.left(bytes));

        emit dataReady(packet.getCMD(), packet.getPayload());

        QLOG_INFO() << "Power Sensor CMD Received | CMD " << packet.getCMD() << " | Size: " << packet.getPayload().size();

        m_data = m_data.mid(bytes + qstrlen(PACKET_END));
    }
}
