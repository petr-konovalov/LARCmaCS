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
	~ReceiverWorker();

public slots:
	void processPendingDatagrams();
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
	void updateDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID);
	void updateGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry);
	void UpdateSSLFPS(const QString & message);

private:
	void init();
	static const QString visionIP;
	QSharedPointer<QUdpSocket> mSocket;
	QSharedPointer<QTimer> mStatisticsTimer;
	QHostAddress mGroupAddress;
	QSharedPointer<SSL_WrapperPacket> mInputPacket;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	bool mIsSimEnabledFlag = false;
};
