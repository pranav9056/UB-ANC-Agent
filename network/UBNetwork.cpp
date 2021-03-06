#include "UBNetwork.h"
#include "UBNetPacket.h"

#include "config.h"
#include "QsLog.h"

#include <QTcpSocket>
#include <QHostAddress>

UBNetwork::UBNetwork(QObject *parent) : QObject(parent),
    m_id(0)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReadyEvent()));
}

void UBNetwork::startNetwork(quint32 id, quint16 port) {
    m_id = id;

    m_socket->connectToHost(QHostAddress::LocalHost, port);
}

void UBNetwork::sendData(quint32 desID, const QByteArray& data) {
    UBNetPacket packet;

    packet.setSrcID(m_id);
    packet.setDesID(desID);
    packet.setPayload(data);

    m_socket->write(packet.packetize().append(PACKET_END));
}

void UBNetwork::dataReadyEvent() {
    m_data += m_socket->readAll();

    while (m_data.contains(PACKET_END)) {
        int bytes = m_data.indexOf(PACKET_END);

        UBNetPacket packet;
        packet.depacketize(m_data.left(bytes));

        if (packet.getDesID() == m_id || packet.getDesID() == BROADCAST_ID) {
            emit dataReady(packet.getSrcID(), packet.getPayload());

            QLOG_INFO() << "Packet Received | From " << packet.getSrcID() << " to " << packet.getDesID() << " | Size: " << packet.getPayload().size();
        }

        m_data = m_data.mid(bytes + qstrlen(PACKET_END));
    }
}
