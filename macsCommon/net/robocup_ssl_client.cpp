// Copyright 2019 Dmitrii Iarosh

#include "robocup_ssl_client.h"

RoboCupSSLClient::RoboCupSSLClient(unsigned short port)
	: groupAddress(QStringLiteral("224.5.23.2"))
{
	_port = port;
	in_buffer = new char[65536];
	packet = new SSL_WrapperPacket();
}


RoboCupSSLClient::~RoboCupSSLClient()
{
	delete[] in_buffer;
}

void RoboCupSSLClient::close() {
	socket.close();
}

bool RoboCupSSLClient::open() {
	close();
	socket.bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress);
	socket.joinMulticastGroup(groupAddress);
	connect(&socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	return true;
}

void RoboCupSSLClient::processPendingDatagrams()
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

bool RoboCupSSLClient::receive(SSL_WrapperPacket ** packet) {
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

