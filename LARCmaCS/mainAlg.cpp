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

#include "mainAlg.h"

MainAlg::MainAlg(SharedRes * sharedRes)
	: mWorker(new MainAlgWorker(sharedRes))
{
	mWorker->moveToThread(&mThread);
	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(this, SIGNAL(MLEvalString(const QString &)), mWorker, SLOT(evalString(const QString &)));
	connect(mWorker, SIGNAL(newPauseState(const QString &)), this, SLOT(receivePauseState(const QString &)));
	connect(mWorker, SIGNAL(sendStatistics(const QString &)), this, SIGNAL(engineStatistics(const QString &)));
	connect(mWorker, SIGNAL(toMatlabConsole(const QString &)), this, SIGNAL(toMatlabConsole(const QString &)));
	connect(this, SIGNAL(updateMatlabDebugFrequency(int)), mWorker, SLOT(setMatlabDebugFrequency(int)));

	connect(mWorker, SIGNAL(newData(const QVector<double> &)), this, SIGNAL(newData(const QVector<double> &)));
	connect(mWorker, SIGNAL(pause(bool)), this, SIGNAL(pause(bool)));

	connect(this, SIGNAL(stopped()), mWorker, SLOT(stop()));

	mThread.start();
}

MainAlg::~MainAlg()
{
	emit stopped();
	mThread.quit();
	mThread.wait();
}

void MainAlg::changeBallStatus(bool status)
{
	emit updateBallStatus(status);
}

void MainAlg::EvalString(const QString & s)
{
	emit MLEvalString(s);
}

void MainAlg::receivePauseState(const QString & state)
{
	emit UpdatePauseState(state);
}
