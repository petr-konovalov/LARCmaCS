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

#include "receiverWorker.h"

#include <QApplication>
#include <QMutex>

#include "constants.h"
#include "packetSSL.h"

const QString ReceiverWorker::visionIP = QStringLiteral("224.5.23.2");

ReceiverWorker::ReceiverWorker()
	: mSocket(this)
	, mStatisticsTimer(this)
	, mGroupAddress(visionIP)
{
	mStatisticsTimer.setInterval(1000);

	connect(&mStatisticsTimer, SIGNAL(timeout()), this, SLOT(formStatistics()));
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

ReceiverWorker::~ReceiverWorker()
{}

void ReceiverWorker::formStatistics()
{
	QString tmp;
	QString ToStatus = "FPS = ";
	tmp.setNum(mPacketsPerSecond);
	ToStatus += tmp;
	mPacketsPerSecond = 0;
	ToStatus += "; Total Packets = ";
	tmp.setNum(mTotalPacketsNum);
	ToStatus += tmp;
	emit updateSSLFPS(ToStatus);
}

void ReceiverWorker::close()
{
	mSocket.close();
	mStatisticsTimer.stop();
	mTotalPacketsNum = 0;
	mPacketsPerSecond = 0;
	for (int i = 0; i < Constants::numOfCameras; i++) {
		emit updateDetection(QSharedPointer<SSL_WrapperPacket>(), i);
	}
	emit updateGeometry(QSharedPointer<SSL_WrapperPacket>());
}

bool ReceiverWorker::open(unsigned short port)
{
	close();
	if (mSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)
			&& mSocket.joinMulticastGroup(mGroupAddress)) {
		mStatisticsTimer.start();
		return true;
	}
	return false;
}

void ReceiverWorker::processPendingDatagrams()
{
	while (mSocket.hasPendingDatagrams()) {
		int datagramSize = static_cast<int>(mSocket.pendingDatagramSize());
		QByteArray datagram;
		datagram.resize(datagramSize);
		mSocket.readDatagram(datagram.data(), datagram.size());
		QSharedPointer<SSL_WrapperPacket> packet(new SSL_WrapperPacket());
		auto parseResult = packet->ParseFromArray(datagram.data(), datagramSize);
		if (!parseResult) {
            //qDebug() << "ERROR: Failed to parse packet from datagram; skipping";
			continue;
		}

		if (!packet->IsInitialized()) {
            //qDebug() << "ERROR: Packet is uninitialized; skipping";
			continue;
		}

		if (packet->has_detection())
			emit updateDetection(packet, packet->detection().camera_id());

		if (packet->has_geometry())
			emit updateGeometry(packet);

		mTotalPacketsNum++;
		mPacketsPerSecond++;
	}
}

void ReceiverWorker::start()
{
	open(Constants::SSLVisionPort);
}

void ReceiverWorker::changeSimulatorMode(bool isSim)
{
	if (isSim != mIsSimEnabledFlag) {
		mIsSimEnabledFlag = isSim;
		if (mIsSimEnabledFlag) {
			close();
			open(Constants::SimVisionPort);
		} else {
			close();
			open(Constants::SSLVisionPort);
		}
	}
}
