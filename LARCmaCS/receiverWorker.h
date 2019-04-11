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

#define NUM_OF_CAMERAS 4

#pragma once

using namespace std;

#include <iostream>
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QTimer>
#include "messages_robocup_ssl_wrapper.pb.h"
#include "packetSSL.h"

#include <time.h>

#define SSL_VISION_PORT 10006
#define SIM_VISION_PORT 10020

class ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();
	QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > getVisionData();
	~ReceiverWorker();

public slots:
	void processPendingDatagrams();
	void swapDataVectors();
	void init();
	void formStatistics();
	bool open(unsigned short port);
	void clearOutput();
	void close();
	void start();
	void ChangeSimulatorMode(bool flag);

signals:
	void clientOpen(unsigned short port);
	void clientClose();
	void clearField();
	void UpdateSSLFPS(QString message);

private:
	static const QString visionIP;
	static const int MaxDataGramSize = 65536;
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
