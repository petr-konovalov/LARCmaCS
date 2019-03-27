// Copyright 2019 Dmitrii Iarosh

#include "robocup_vision_client.h"

RoboCupVisionClient::RoboCupVisionClient(unsigned short port)
	: groupAddress(visionIP)
{
	_port = port;
	packet = new SSL_WrapperPacket();
}

RoboCupVisionClient::~RoboCupVisionClient(){}

void RoboCupVisionClient::close() {
	socket.close();
}

bool RoboCupVisionClient::open()
{
	close();
	socket.bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress);
	socket.joinMulticastGroup(groupAddress);
	connect(&socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	return true;
}

void RoboCupVisionClient::processPendingDatagrams()
{
	QByteArray datagram;
	int datagramSize;
	while (socket.hasPendingDatagrams()) {
		datagram.resize(int(socket.pendingDatagramSize()));
		datagramSize = int(socket.pendingDatagramSize());
		socket.readDatagram(datagram.data(), datagram.size());
		packet->ParseFromArray(datagram.data(), datagramSize);
		mutex.lock();
		newPacket = true;
		outputPacket = packet;
		mutex.unlock();
		packet = new SSL_WrapperPacket();
	}
}


bool RoboCupVisionClient::receive(SSL_WrapperPacket ** packet) {
	mutex.lock();
	if (newPacket) {
		*packet = outputPacket;
		newPacket = false;
		mutex.unlock();
		return true;
	}
	mutex.unlock();
	return false;
}
