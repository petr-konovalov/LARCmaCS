// Copyright 2019 Anastasiia Kornilova

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <QtNetwork>

#include "robotReceiverWorker.h"
#include "message.h"
#include "constants.h"

const QString RobotReceiverWorker::mSocketIp = QStringLiteral("255.255.255.255");

RobotReceiverWorker::RobotReceiverWorker()
	: mUdpSocket(this)
{
	mBarrierState.resize(Constants::maxRobotsInTeam);
	mKickerChargeStatus.resize(Constants::maxRobotsInTeam);
	mConnectionState.resize(Constants::maxRobotsInTeam);
	mChargeLevel.resize(Constants::maxRobotsInTeam);
}

RobotReceiverWorker::~RobotReceiverWorker()
{
	mUdpSocket.close();
}

void RobotReceiverWorker::start()
{
	mUdpSocket.bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ShareAddress);
	mUdpSocket.joinMulticastGroup(QHostAddress(mSocketIp));
	connect(&mUdpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void RobotReceiverWorker::processPendingDatagrams()
{
	QByteArray datagram;

	while (mUdpSocket.hasPendingDatagrams()) {
		datagram.resize(static_cast<int>(mUdpSocket.pendingDatagramSize()));
		mUdpSocket.readDatagram(datagram.data(), datagram.size());
		if (datagram.size() != mDatagramSize) {
            qDebug() << "Packet of incorrect length was received in RobotReceiver";
			continue;
		}

		for (int i = 0; i < mRobotsInPacket; i++) {
			mChargeLevel[i] = datagram[1 + i * mOnePacketLength];
			mBarrierState[i] = datagram[17 + i * mOnePacketLength];
			mKickerChargeStatus[i] = datagram[18 + i * mOnePacketLength];
			mConnectionState[i] = datagram[19 + i * mOnePacketLength];
		}

		emit newBarrierState(mBarrierState);
		emit newKickerChargeStatus(mKickerChargeStatus);
		emit newConnectionState(mConnectionState);
		emit newChargeLevel(mChargeLevel);
	}
}
