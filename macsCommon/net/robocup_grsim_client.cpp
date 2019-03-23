// Copyright 2019 Dmitrii Iarosh

#include "robocup_grsim_client.h"

RoboCupGrSimClient::RoboCupGrSimClient(unsigned short port)
	: groupAddress(QStringLiteral("224.5.23.2"))
{
	_port = port;
	in_buffer = new char[65536];
	packet = new SSL_WrapperPacket();
}


RoboCupGrSimClient::~RoboCupGrSimClient()
{
	delete[] in_buffer;
}

void RoboCupGrSimClient::close() {
	socket.close();
}

bool RoboCupGrSimClient::open()
{
	close();
	socket.bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress);
	socket.joinMulticastGroup(groupAddress);
	connect(&socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	return true;
}

void RoboCupGrSimClient::processPendingDatagrams()
{
	cout << "@@\n";
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


bool RoboCupGrSimClient::receive(SSL_WrapperPacket ** packet) {
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
