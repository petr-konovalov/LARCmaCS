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

RobotReceiverWorker::RobotReceiverWorker(): groupAddress4(QStringLiteral("192.168.1.255"))
{
	udpSocket4.bind(QHostAddress::AnyIPv4, 57000, QUdpSocket::ShareAddress);
	udpSocket4.joinMulticastGroup(groupAddress4);

	connect(&udpSocket4, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void RobotReceiverWorker::start()
{

}

void RobotReceiverWorker::stop()
{
	close();
	emit finished();
}

void RobotReceiverWorker::close()
{

}

void RobotReceiverWorker::processPendingDatagrams()
{
	QByteArray datagram;

	while (udpSocket4.hasPendingDatagrams()) {
		datagram.resize(int(udpSocket4.pendingDatagramSize()));
		udpSocket4.readDatagram(datagram.data(), datagram.size());
		ballStatuses[datagram.at(29)] = datagram.at(4);
		auto found = std::find(ballStatuses.begin(), ballStatuses.end(), 1);
		//emit ballStatus(found != ballStatuses.end());
	}
}
