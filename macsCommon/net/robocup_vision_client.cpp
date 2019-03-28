// Copyright 2019 Dmitrii Iarosh

#include "robocup_vision_client.h"

RoboCupVisionClient::RoboCupVisionClient(unsigned short port)
	: groupAddress(visionIP)
{
	_port = port;
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		newPacket[i] = false;
	}
	inputPacket = new SSL_WrapperPacket();
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(processFrame()));
}

RoboCupVisionClient::~RoboCupVisionClient()
{
	timer->stop();
	delete timer;
}

void RoboCupVisionClient::close()
{
	timer->stop();
	socket.close();
}

bool RoboCupVisionClient::open()
{
	close();
	socket.bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress);
	socket.joinMulticastGroup(groupAddress);
	connect(&socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	timer->start();
	return true;
}

void RoboCupVisionClient::processPendingDatagrams()
{
	unsigned int camID = 0;
	QByteArray datagram;
	int datagramSize;
	while (socket.hasPendingDatagrams()) {
		datagram.resize(int(socket.pendingDatagramSize()));
		datagramSize = int(socket.pendingDatagramSize());
		socket.readDatagram(datagram.data(), datagram.size());
		inputPacket->ParseFromArray(datagram.data(), datagramSize);

		if(inputPacket->has_detection()) {
			camID = inputPacket->detection().camera_id();
		} else {
			camID = 0;
		}

		mutex.lock();
		newPacket[camID] = true;
		outputPacket[camID] = inputPacket;
		mutex.unlock();
		inputPacket = new SSL_WrapperPacket();
	}
}

void RoboCupVisionClient::processFrame()
{
	SSL_WrapperPacket * procPacket;
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		mutex.lock();
		if (newPacket[i]) {
			procPacket = outputPacket[i];
			newPacket[i] = false;
			mutex.unlock();
			emit processPacket(procPacket);
		} else {
			mutex.unlock();
		}
	}
}
