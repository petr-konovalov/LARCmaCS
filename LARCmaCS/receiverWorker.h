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

#pragma once

using namespace std;

#include <iostream>
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QTimer>
#include "constants.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "packetSSL.h"

class ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();
	const QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > & getVisionData();
	~ReceiverWorker();

public slots:
	void processPendingDatagrams();
	void swapDataVectors();
	void formStatistics();
	bool open(unsigned short port);
	void clearOutput();
	void close();
	void start();
	void stop();
	void ChangeSimulatorMode(bool flag);

signals:
	void clientOpen(unsigned short port);
	void clientClose();
	void clearField();
	void finished();
	void UpdateSSLFPS(const QString & message);

private:
	void init();
	QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > outputVisionData;
	static const QString visionIP;
	QSharedPointer<QUdpSocket> mSocket;
	QSharedPointer<QTimer> mStatisticsTimer;
	bool mClearFlag = false;
	QHostAddress mGroupAddress;
	QMutex mMutex;
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > mDetectionPacket;
	QSharedPointer<SSL_WrapperPacket> mGeometryPacket;
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > mOutputDetectionPacket;
	QSharedPointer<SSL_WrapperPacket> mOutputGeometryPacket;
	QSharedPointer<SSL_WrapperPacket> mInputPacket;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	bool mIsSimEnabledFlag = false;
};
