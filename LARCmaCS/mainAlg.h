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

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include <QFile>
#include <iostream>

#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "mainAlgWorker.h"
#include "receiver.h"
#include "messages_robocup_ssl_wrapper.pb.h"

using namespace std;

struct MainAlg : public QObject
{
	Q_OBJECT
private:
	QSharedPointer<PacketSSL> mPacketSSL;
	Receiver * mReceiver;
	SSL_DetectionFrame mDetection;
	MainAlgWorker * mWorker;
	QThread * mThread;
public:
	bool getIsSimEnabledFlag();
	explicit MainAlg();
	~MainAlg();
	void init(Receiver * receiver);
	void start();
	void stop();

public slots:
	void EvalString(const QString & s);
	void moveToConnector(int N, const QByteArray & command);
	void moveToSimConnector(const QByteArray & command);
	void receivePauseState(const QString & state);
	void setEnableSimFlag(bool flag);
	void changeBallStatus(bool status);
	void loadVisionData();
	void sendStatistics(const QString & statistics);

signals:
	void updateBallStatus(bool status);
	void MLEvalString(const QString & s);
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void updateEnableSimFlag(bool flag);
	void StatusMessage(const QString & status);
	void UpdatePauseState(const QString & state);
	void wstop();
};
