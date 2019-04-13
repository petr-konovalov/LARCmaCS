#include "mainAlg.h"

#include <iostream>
#include <windows.h>

#include <QtWidgets/QApplication>

using namespace std;

#include "message.h"

MainAlg::MainAlg(){}

MainAlg::~MainAlg()
{
	stop();
	thread.wait(100);
	thread.terminate();
	thread.wait(100);
}

void MainAlg::init(Receiver * receiver)
{
	mReceiver = receiver;
	mPacketSSL = QSharedPointer<PacketSSL>(new PacketSSL());
	worker.moveToThread(&thread);
	cout << "Init mainAlg ok" << endl;
	connect(this, SIGNAL(MLEvalString(QString)), &worker, SLOT(EvalString(QString)));
	connect(&worker, SIGNAL(getDataFromReceiver()), this, SLOT(loadVisionData()), Qt::DirectConnection);
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
	connect(&worker, SIGNAL(newPauseState(QString)), this, SLOT(receivePauseState(QString)));
	connect(&worker, SIGNAL(sendStatistics(QString)), this, SLOT(sendStatistics(QString)));
	connect(&worker, SIGNAL(sendToConnector(int, const QByteArray &)), this, SLOT(moveToConnector(int, const QByteArray &)));
	connect(&worker, SIGNAL(sendToSimConnector(const QByteArray &)), this, SLOT(moveToSimConnector(const QByteArray &)));
	connect(this, SIGNAL(updateEnableSimFlag(bool)), &worker, SLOT(setEnableSimFlag(bool)));
	connect(this, SIGNAL(updateBallStatus(bool)), &worker, SLOT(changeBallStatus(bool)));
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
}

void MainAlg::sendStatistics(QString statistics)
{
	emit StatusMessage(statistics);
}

void MainAlg::changeBallStatus(bool status)
{
	emit updateBallStatus(status);
}

bool MainAlg::getIsSimEnabledFlag()
{
	return worker.getIsSimEnabledFlag();
}

void MainAlg::setEnableSimFlag(bool flag)
{
	emit updateEnableSimFlag(flag);
}

void MainAlg::EvalString(QString s)
{
	emit MLEvalString(s);
}

void MainAlg::moveToConnector(int N, const QByteArray & command)
{
	emit sendToConnector(N, command);
}

void MainAlg::moveToSimConnector(const QByteArray & command)
{
	emit sendToSimConnector(command);
}

void MainAlg::loadVisionData()
{
	mReceiver->swapDataVectors();
	QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > tmp = mReceiver->getVisionData();
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detectionPackets = tmp->first;
	int balls_n, idCam, robots_blue_n, robots_yellow_n;
	SSL_DetectionBall ball;

	QSharedPointer<SSL_WrapperPacket> packet;
	for (int i = 0; i < TEAM_COUNT / 4; i++) {
			mPacketSSL->robots_blue[i] = 0;
			mPacketSSL->robots_yellow[i] = 0;
	}
	mPacketSSL->balls[0] = 0;
	for (int i = 0; i < detectionPackets->size(); i++) {
		packet = detectionPackets->at(i);
		if (packet.isNull()) {
			continue;
		}
		SSL_DetectionRobot robot;
		SSL_DetectionFrame detection = packet->detection();

		idCam = detection.camera_id() + 1;
		balls_n = detection.balls_size();

		// [Start] Ball info
		if (balls_n != 0) {
			mPacketSSL->balls[0] = idCam;
			ball = detection.balls(0);
			mPacketSSL->balls[1] = ball.x();
			mPacketSSL->balls[2] = ball.y();
		}
		// [End] Ball info

		// [Start] Robot info
		robots_blue_n = detection.robots_blue_size();
		robots_yellow_n = detection.robots_yellow_size();

		for (int i = 0; i < robots_blue_n; i++) {
			robot = detection.robots_blue(i);
			mPacketSSL->robots_blue[robot.robot_id()] = idCam;
			mPacketSSL->robots_blue[robot.robot_id() + 12] = robot.x();
			mPacketSSL->robots_blue[robot.robot_id() + 24] = robot.y();
			mPacketSSL->robots_blue[robot.robot_id() + 36] = robot.orientation();
		}

		for (int i = 0; i < robots_yellow_n; i++) {
			robot = detection.robots_yellow(i);
			mPacketSSL->robots_yellow[robot.robot_id()] = idCam;
			mPacketSSL->robots_yellow[robot.robot_id() + 12] = robot.x();
			mPacketSSL->robots_yellow[robot.robot_id() + 24] = robot.y();
			mPacketSSL->robots_yellow[robot.robot_id() + 36] = robot.orientation();
		}
		// [End] Robot info
	}

	worker.setPacketSSL(mPacketSSL);
}

void MainAlg::receivePauseState(QString state)
{
	emit UpdatePauseState(state);
}

void MainAlg::start()
{
	thread.start();
	cout << "Thread start" << endl;
	emit wstart();
}

void MainAlg::stop()
{
	emit wstop();
}
