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
#include <QtWidgets/QApplication>

const QString ReceiverWorker::visionIP = QStringLiteral("224.5.23.2");

ReceiverWorker::ReceiverWorker()
	: mGroupAddress(visionIP)
{
	mDetectionPacket = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >(new QVector<QSharedPointer<SSL_WrapperPacket> >());
	mDetectionPacket->resize(Constants::numOfCameras);
	mGeometryPacket = QSharedPointer<SSL_WrapperPacket>();
	mInputPacket = QSharedPointer<SSL_WrapperPacket>(new SSL_WrapperPacket());
	outputVisionData = QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > >(new pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> >());
}

void ReceiverWorker::init()
{
	mStatisticsTimer = QSharedPointer<QTimer>(new QTimer());
	mSocket = QSharedPointer<QUdpSocket>(new QUdpSocket());
	mStatisticsTimer->setInterval(1000);
	connect(mStatisticsTimer.data(), SIGNAL(timeout()), this, SLOT(formStatistics()));
	connect(mSocket.data(), SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

ReceiverWorker::~ReceiverWorker()
{
	close();
}

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
	emit UpdateSSLFPS(ToStatus);
}

void ReceiverWorker::close()
{
	mSocket->close();
	mStatisticsTimer->stop();
	clearOutput();
	mTotalPacketsNum = 0;
	mPacketsPerSecond = 0;
}

void ReceiverWorker::clearOutput()
{
	mMutex.lock();
	for (int i = 0; i < Constants::numOfCameras; i++) {
		mDetectionPacket->replace(i, QSharedPointer<SSL_WrapperPacket>());
	}
	mGeometryPacket = QSharedPointer<SSL_WrapperPacket>();
	mMutex.unlock();
	emit clearField();
}

void ReceiverWorker::stop()
{
	close();
	emit finished();
}

bool ReceiverWorker::open(unsigned short port)
{
	close();
	if (mSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress) && mSocket->joinMulticastGroup(mGroupAddress)) {
		mStatisticsTimer->start();
		return true;
	}
	return false;
}

const QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > & ReceiverWorker::getVisionData()
{
	return outputVisionData;
}

void ReceiverWorker::swapDataVectors()
{
	mMutex.lock();
	outputVisionData->first = mDetectionPacket;
	outputVisionData->second = mGeometryPacket;
	mMutex.unlock();
}

void ReceiverWorker::processPendingDatagrams()
{
	unsigned int camID = 0;
	QByteArray datagram;
	int datagramSize;
	while (mSocket->hasPendingDatagrams()) {
		datagramSize = static_cast<int>(mSocket->pendingDatagramSize());
		datagram.resize(datagramSize);
		mSocket->readDatagram(datagram.data(), datagram.size());
		mInputPacket->ParseFromArray(datagram.data(), datagramSize);
		if (mInputPacket->has_detection()) {
			camID = mInputPacket->detection().camera_id();
			mMutex.lock();
			mDetectionPacket->replace(camID, mInputPacket);
			mMutex.unlock();
		} else {
			mMutex.lock();
			mGeometryPacket = mInputPacket;
			mMutex.unlock();
		}
		mTotalPacketsNum++;
		mPacketsPerSecond++;
		mInputPacket = QSharedPointer<SSL_WrapperPacket>(new SSL_WrapperPacket());
	}
}

void ReceiverWorker::start()
{
	init();
	cout << "Receiver worker start" << endl;
	connect(this, SIGNAL(clientOpen(unsigned short)), this, SLOT(open(unsigned short)));
	connect(this, SIGNAL(clientClose()), this, SLOT(close()));
	emit clientOpen(Constants::SSLVisionPort);
}

void ReceiverWorker::ChangeSimulatorMode(bool flag)
{
	if (flag != mIsSimEnabledFlag) {
		mIsSimEnabledFlag = flag;
		if (mIsSimEnabledFlag) {
			emit clientClose();
			emit clientOpen(Constants::SimVisionPort);
		} else {
			emit clientClose();
			emit clientOpen(Constants::SSLVisionPort);
		}
	}
}
