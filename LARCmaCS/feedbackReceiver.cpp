// Copyright 2019 Dmitrii Iarosh, Anastasiia Kornilova

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "feedbackReceiver.h"

#include <QDebug>

FeedbackReceiver::FeedbackReceiver(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
{
    mWorker = new FeedbackReceiverWorker();
	mWorker->moveToThread(&mThread);

	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

    connect(mWorker, SIGNAL(newRobotFeedback(const QSharedPointer<RobotFeedback> &))
                , this, SLOT(changeRobotFeedback(const QSharedPointer<RobotFeedback> &)));

    connect(this, SIGNAL(changeNetAddress(const QString &, int, int, const QString &)),
            mWorker, SLOT(changeNetAddress(const QString &, int, int, const QString &)));

    mThread.start();
}

FeedbackReceiver::~FeedbackReceiver()
{
	mThread.quit();
	mThread.wait();
}

void FeedbackReceiver::changeRobotFeedback(const QSharedPointer<RobotFeedback> & robotFeedback)
{
    qDebug() << "Received robot feedbach" << robotFeedback << endl;
    //qDebug() << robotFeedback->has_dribbler_ball_contact() << endl;
}

void FeedbackReceiver::changeSimulatorMode(bool, const QString & ip, int port, int portYellow, const QString & netInterface)
{
    emit changeNetAddress(ip, port, portYellow, netInterface);
}
