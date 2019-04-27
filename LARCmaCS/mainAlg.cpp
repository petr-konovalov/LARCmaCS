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
	: mSharedRes(sharedRes)
	, mWorker(new MainAlgWorker())
{
	mWorker->moveToThread(&mThread);
	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(this, SIGNAL(MLEvalString(const QString &)), mWorker, SLOT(evalString(const QString &)));
	connect(mWorker, SIGNAL(getDataFromReceiver()), this, SLOT(loadVisionData()), Qt::DirectConnection);
	connect(mWorker, SIGNAL(newPauseState(const QString &)), this, SLOT(receivePauseState(const QString &)));
	connect(mWorker, SIGNAL(sendStatistics(const QString &)), this, SIGNAL(engineStatistics(const QString &)));
	connect(mWorker, SIGNAL(sendToConnector(int, const QByteArray &)),
			this, SIGNAL(sendToConnector(int, const QByteArray &)));
	connect(mWorker, SIGNAL(sendToSimConnector(const QByteArray &)),
			this, SIGNAL(sendToSimConnector(const QByteArray &)));
	connect(this, SIGNAL(updateEnableSimFlag(bool)), mWorker, SLOT(setEnableSimFlag(bool)));
	connect(this, SIGNAL(updateBallStatus(bool)), mWorker, SLOT(changeBallStatus(bool)));
	connect(mWorker, SIGNAL(toMatlabConsole(const QString &)), this, SIGNAL(toMatlabConsole(const QString &)));
	connect(this, SIGNAL(updateMatlabDebugFrequency(int)), mWorker, SLOT(setMatlabDebugFrequency(int)));

	mThread.start();
}

MainAlg::~MainAlg()
{
	mThread.quit();
	mThread.wait();
}

void MainAlg::changeBallStatus(bool status)
{
	emit updateBallStatus(status);
}

bool MainAlg::getIsSimEnabledFlag()
{
	return mWorker->getIsSimEnabledFlag();
}

void MainAlg::setEnableSimFlag(bool flag)
{
	emit updateEnableSimFlag(flag);
}

void MainAlg::EvalString(const QString & s)
{
	emit MLEvalString(s);
}

void MainAlg::loadVisionData()
{
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detectionPackets = mSharedRes->getDetection();
	int balls_n, idCam, robots_blue_n, robots_yellow_n;
	SSL_DetectionBall ball;

	QSharedPointer<SSL_WrapperPacket> packet;

	QSharedPointer<PacketSSL> mPacketSSL = QSharedPointer<PacketSSL>(new PacketSSL());
	for (int i = 0; i < Constants::maxRobotsInTeam; i++) {
			mPacketSSL->robots_blue[i] = 0;
			mPacketSSL->robots_yellow[i] = 0;
	}
	mPacketSSL->balls[0] = 0;
	if (detectionPackets.isNull())
	{
		return;
	}
	for (int i = 0; i < detectionPackets->size(); i++) {
		packet = detectionPackets->at(i);
		if (packet.isNull()) {
			continue;
		}
		SSL_DetectionRobot robot;
		mDetection.Clear();
		mDetection = packet->detection();

		idCam = mDetection.camera_id() + 1;
		balls_n = mDetection.balls_size();

		// [Start] Ball info
		if (balls_n != 0) {
			mPacketSSL->balls[0] = idCam;
			ball = mDetection.balls(0);
			mPacketSSL->balls[1] = ball.x();
			mPacketSSL->balls[2] = ball.y();
		}
		// [End] Ball info

		// [Start] Robot info
		robots_blue_n = mDetection.robots_blue_size();
		robots_yellow_n = mDetection.robots_yellow_size();

		for (int i = 0; i < robots_blue_n; i++) {
			robot = mDetection.robots_blue(i);
			if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
				mPacketSSL->robots_blue[robot.robot_id()] = idCam;
				mPacketSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam] = robot.x();
				mPacketSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 2] = robot.y();
				mPacketSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 3] = robot.orientation();
			} else {
				if (robot.has_robot_id()) {
					cout << robot.robot_id() << " blue" << endl;
				}
			}
		}

		for (int i = 0; i < robots_yellow_n; i++) {
			robot = mDetection.robots_yellow(i);
			if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
				mPacketSSL->robots_yellow[robot.robot_id()] = idCam;
				mPacketSSL->robots_yellow[robot.robot_id() + 12] = robot.x();
				mPacketSSL->robots_yellow[robot.robot_id() + 24] = robot.y();
				mPacketSSL->robots_yellow[robot.robot_id() + 36] = robot.orientation();
			} else {
				if (robot.has_robot_id()) {
					cout << robot.robot_id() << " yellow" << endl;
				}
			}
		}
		// [End] Robot info
	}

	mWorker->setPacketSSL(mPacketSSL);
}

void MainAlg::receivePauseState(const QString & state)
{
	emit UpdatePauseState(state);
}
