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

RobotReceiverWorker::RobotReceiverWorker()
	: mUdpSocket(this)
	, mGroupAddress(QStringLiteral("192.168.1.255"))
{}

RobotReceiverWorker::~RobotReceiverWorker()
{
	mUdpSocket.close();
}

void RobotReceiverWorker::start()
{
	mUdpSocket.bind(QHostAddress::AnyIPv4, 57000, QUdpSocket::ShareAddress);
	mUdpSocket.joinMulticastGroup(mGroupAddress);
	connect(&mUdpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void RobotReceiverWorker::processPendingDatagrams()
{
	QByteArray datagram;

	while (mUdpSocket.hasPendingDatagrams()) {
		datagram.resize(static_cast<int>(mUdpSocket.pendingDatagramSize()));
		mUdpSocket.readDatagram(datagram.data(), datagram.size());
		if (datagram.size() != 50) {
			continue;
		}
		quint32 crc = Message::calculateCRC(datagram, 46);
		if (crc != ((quint32)(datagram.at(47) << 24) + (quint32)(datagram.at(48) << 16)
					+ (quint32)(datagram.at(49) << 8) + (quint32)datagram.at(50))) {
			continue;
		}
		QString ip = QString::number((quint32)datagram.at(26)) + "."
				+ QString::number((quint32)datagram.at(27)) + "." + QString::number((quint32)datagram.at(28))
				+ "." + QString::number((quint32)datagram.at(29));
		bool isBallInside = datagram.at(4) == 0 ? false : true;
		emit setBallInsideData(ip, isBallInside);
	}
}
