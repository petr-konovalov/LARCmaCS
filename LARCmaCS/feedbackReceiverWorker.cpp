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

#include "feedbackReceiverWorker.h"
#include "message.h"
#include "constants.h"

const QString FeedbackReceiverWorker::defaultIp = QStringLiteral("127.0.0.1");
const QString FeedbackReceiverWorker::defaultInterface = QStringLiteral("eth0");
const int FeedbackReceiverWorker::defaultPort = 10301;

FeedbackReceiverWorker::FeedbackReceiverWorker()
    :mUdpSocket(this)
{}

FeedbackReceiverWorker::~FeedbackReceiverWorker()
{
    close();
}

void FeedbackReceiverWorker::start()
{
    open(QHostAddress(defaultIp), defaultPort, defaultInterface);
    connect(&mUdpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}


void FeedbackReceiverWorker::close()
{
    mUdpSocket.close();
}

// TODO: Again code duplication. Common class for UDP sockets
QNetworkInterface FeedbackReceiverWorker::getInterfaceByName(const QString &netInterface)
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.length(); ++i) {
        if (interfaces.at(i).name() == netInterface) {
            qInfo() << "Found interface " << netInterface;
            return interfaces.at(i);
        };
    }
    qInfo() << "ERROR: No interface found for " << netInterface << ". Using first interface";
    return interfaces.at(0);
}

bool FeedbackReceiverWorker::open(const QHostAddress ip, unsigned short port, const QString &netInterface)
{
    close();
    QNetworkInterface interface = getInterfaceByName(netInterface);
    if (mUdpSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)
            && mUdpSocket.joinMulticastGroup(ip, interface)) {
        return true;
    }
    return false;
}

void FeedbackReceiverWorker::processPendingDatagrams()
{
	QByteArray datagram;

    while (mUdpSocket.hasPendingDatagrams()) {
        qInfo() << "Read robot feedback" << endl;
        int datagramSize = static_cast<int>(mUdpSocket.pendingDatagramSize());
        QByteArray datagram;
        datagram.resize(datagramSize);
        mUdpSocket.readDatagram(datagram.data(), datagram.size());
        QSharedPointer<RobotFeedback> packet(new RobotFeedback());
        auto parseResult = packet->ParseFromArray(datagram.data(), datagramSize);
        if (!parseResult) {
            qInfo() << "ERROR: Failed to parse packet from datagram; skipping";
            continue;
        }

        if (!packet->IsInitialized()) {
            qInfo() << "ERROR: Packet is uninitialized; skipping";
            continue;
        }
        qInfo() << "Sending robot feedback" << endl;
        emit newRobotFeedback(packet);
    }
}

void FeedbackReceiverWorker::changeNetAddress(const QString & ip, int port, int portYellow, const QString & netInterface)
{
    qInfo() << "Updating feedback receiver address to" << ip << "and interface to" << netInterface;
    open(QHostAddress(ip), port, netInterface);
}
