// Copyright 2019 Dmitrii Iarosh

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "robocup_vision_client.h"

const QString RoboCupVisionClient::visionIP = QStringLiteral("224.5.23.2");

RoboCupVisionClient::RoboCupVisionClient()
	: mGroupAddress(visionIP)
{
	mOutputPacket.resize(NUM_OF_CAMERAS);
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		mNewPacket[i] = false;
	}
	mInputPacket = new SSL_WrapperPacket();
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(processFrame()));
	mTimer.setInterval(UPDATE_INTERVAL);
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

RoboCupVisionClient::~RoboCupVisionClient()
{
	mTimer.stop();
}

void RoboCupVisionClient::close()
{
	mTimer.stop();
	mSocket.close();
	emit socketClosed();
}

void RoboCupVisionClient::clearOutput()
{
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		mOutputPacket[i] = nullptr;
		mNewPacket[i] = false;
	}
	mGeometryPacket = nullptr;
	mNewGeometryPacket = false;
}

bool RoboCupVisionClient::open(unsigned short port)
{
	close();
	if (mSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress) && mSocket.joinMulticastGroup(mGroupAddress)) {
		mTimer.start();
		return true;
	}
	return false;
}

void RoboCupVisionClient::processPendingDatagrams()
{
	unsigned int camID = 0;
	QByteArray datagram;
	int datagramSize;
	while (mSocket.hasPendingDatagrams()) {
		datagramSize = static_cast<int>(mSocket.pendingDatagramSize());
		datagram.resize(datagramSize);
		mSocket.readDatagram(datagram.data(), datagram.size());
		mInputPacket->ParseFromArray(datagram.data(), datagramSize);

		if (mInputPacket->has_detection()) {
			camID = mInputPacket->detection().camera_id();
			mMutex.lock();
			mNewPacket[camID] = true;
			mOutputPacket[camID] = mInputPacket;
			mMutex.unlock();
		} else {
			mMutex.lock();
			mNewGeometryPacket = true;
			mGeometryPacket = mInputPacket;
			mMutex.unlock();
		}		
		mInputPacket = new SSL_WrapperPacket();
	}
}

void RoboCupVisionClient::processFrame()
{
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		mMutex.lock();
		if (mNewPacket[i]) {
			SSL_WrapperPacket * procPacket = mOutputPacket[i];
			mNewPacket[i] = false;
			mMutex.unlock();
			emit processPacket(procPacket);
		} else {
			mMutex.unlock();
		}
	}
	mMutex.lock();
	if (mNewGeometryPacket) {
		SSL_WrapperPacket * procPacket = mGeometryPacket;
		mNewGeometryPacket = false;
		mMutex.unlock();
		emit processPacket(procPacket);
	} else {
		mMutex.unlock();
	}
}
