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

#include <QThread>

#include "mainAlgWorker.h"
#include "sharedRes.h"

#include "messages_robocup_ssl_wrapper.pb.h"

using namespace std;

class MainAlg : public QObject
{
	Q_OBJECT

public:
	explicit MainAlg(SharedRes * sharedRes);
	~MainAlg();

public slots:
	void EvalString(const QString & s);
	void receivePauseState(const QString & state);
	void changeBallStatus(bool status);
	void loadVisionData();

signals:
	void updateBallStatus(bool status);
	void updateMatlabDebugFrequency(int frequency);
	void MLEvalString(const QString & s);
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void updateEnableSimFlag(bool flag);
	void engineStatistics(const QString & status);
	void UpdatePauseState(const QString & state);
	void toMatlabConsole(const QString & str);
	void connectorChanged(bool, const QString &, int);
	void newData(const QVector<double> & data);
	void pause(bool status);
	void stopped();

private:
	SharedRes * mSharedRes;
	SSL_DetectionFrame mDetection;
	MainAlgWorker * mWorker;
	QThread mThread;
};
