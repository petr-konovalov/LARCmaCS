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
	: mGroupAddress(visionIP){}

void ReceiverWorker::init()
{
	mStatisticsTimer = QSharedPointer<QTimer>(new QTimer());
	mSocket = QSharedPointer<QUdpSocket>(new QUdpSocket());
	qRegisterMetaType<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > >("QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >");
	qRegisterMetaType<QSharedPointer<SSL_WrapperPacket> >("QSharedPointer<SSL_WrapperPacket>");
	mDetectionPacket = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >(new QVector<QSharedPointer<SSL_WrapperPacket> >());
	mOutputDetectionPacket = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >(new QVector<QSharedPointer<SSL_WrapperPacket> >());
	mDetectionPacket->resize(NUM_OF_CAMERAS);
	mOutputDetectionPacket->resize(NUM_OF_CAMERAS);
	mGeometryPacket = QSharedPointer<SSL_WrapperPacket>();
	mOutputGeometryPacket = QSharedPointer<SSL_WrapperPacket>();
	mInputPacket = QSharedPointer<SSL_WrapperPacket>(new SSL_WrapperPacket());
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
	for (int i = 0; i < NUM_OF_CAMERAS; i++) {
		mDetectionPacket->replace(i, QSharedPointer<SSL_WrapperPacket>());
	}
	mGeometryPacket = QSharedPointer<SSL_WrapperPacket>();
	mClearFlag = true;
	mMutex.unlock();
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

QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > ReceiverWorker::getVisionData()
{
	QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > res = QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > >(new pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> >());
	mMutex.lock();
	res->first = mOutputDetectionPacket;
	res->second = mOutputGeometryPacket;
	mMutex.unlock();
	return res;
}

void ReceiverWorker::swapDataVectors()
{
	QSharedPointer<SSL_WrapperPacket> tmpGeometry = mOutputGeometryPacket;
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > tmpDetection = mOutputDetectionPacket;
	mMutex.lock();
	mOutputGeometryPacket = mGeometryPacket;
	mOutputDetectionPacket = mDetectionPacket;
	if (!mClearFlag) {
		mDetectionPacket = tmpDetection;
		mGeometryPacket = tmpGeometry;
	} else {
		emit clearField();
		mClearFlag = false;
	}
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
	cout << "Receiver worker start" << endl;
	connect(this, SIGNAL(clientOpen(unsigned short)), this, SLOT(open(unsigned short)));
	connect(this, SIGNAL(clientClose()), this, SLOT(close()));
	emit clientOpen(SSL_VISION_PORT);
}

void ReceiverWorker::ChangeSimulatorMode(bool flag)
{
	if (flag != mIsSimEnabledFlag) {
		mIsSimEnabledFlag = flag;
		if (mIsSimEnabledFlag) {
			emit clientClose();
			emit clientOpen(SIM_VISION_PORT);
		} else {
			emit clientClose();
			emit clientOpen(SSL_VISION_PORT);
		}
	}
}
