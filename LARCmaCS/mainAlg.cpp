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

void MainAlg::init()
{
	worker.moveToThread(&thread);
	cout << "Init mainAlg ok" << endl;
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
	connect(&worker, SIGNAL(newPauseState(QString)), this, SLOT(receivePauseState(QString)));
	connect(&worker, SIGNAL(getDataFromReceiver()), this, SLOT(getDataFromReceiver()), Qt::DirectConnection);
	connect(this, SIGNAL(processPacket(PacketSSL &)), &worker, SLOT(processPacket(PacketSSL &)), Qt::DirectConnection);
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
	statisticTimer.setInterval(1000);
	connect(&statisticTimer, SIGNAL(timeout()), this, SLOT(formStatistics()));
	connect(&statisticTimer, SIGNAL(timeout()), &worker, SLOT(updatePauseState()));
	statisticTimer.start();
}

void MainAlg::getDataFromReceiver()
{
	emit askReceiverForData();
}

void MainAlg::receiveVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detectionPackets, QSharedPointer<SSL_WrapperPacket> geometryPackets)
{
	int balls_n = 0;

	SSL_DetectionBall ball;
	SSL_DetectionFrame detection;

	QSharedPointer<SSL_WrapperPacket> packet = detectionPackets->at(0); // CRITICAL TODO: expand algos to all cameras

	int robots_blue_n = 0;
	int robots_yellow_n = 0;
	SSL_DetectionRobot robot;

	int idCam = 0;

	packetsPerSecond++;
	totalPacketsNum++;
	//cout << "Num RECEIVER:" << packetsNum << endl;

	detection = packet->detection();

	idCam = detection.camera_id() + 1;
	balls_n = detection.balls_size();

	// [Start] Ball info
	if (balls_n != 0) {
		packetssl.balls[0] = idCam;
		ball = detection.balls(0);
		packetssl.balls[1] = ball.x();
		packetssl.balls[2] = ball.y();
	} else
		packetssl.balls[0] = 0;

	robots_blue_n = detection.robots_blue_size();
	robots_yellow_n = detection.robots_yellow_size();
	// [End] Ball info

	// [Start] Robot info
	for (int i = 0; i < TEAM_COUNT / 4; i++)
		if(packetssl.robots_blue[i] == idCam)
			packetssl.robots_blue[i] = 0;

	for (int i = 0; i < robots_blue_n; i++) {
		robot = detection.robots_blue(i);
		packetssl.robots_blue[robot.robot_id()] = idCam;
		packetssl.robots_blue[robot.robot_id() + 12] = robot.x();
		packetssl.robots_blue[robot.robot_id() + 24] = robot.y();
		packetssl.robots_blue[robot.robot_id() + 36] = robot.orientation();
	}

	for (int i = 0; i < TEAM_COUNT / 4; i++)
		if(packetssl.robots_yellow[i] == idCam)
			packetssl.robots_yellow[i] = 0;

	for (int i = 0; i < robots_yellow_n; i++) {
		robot = detection.robots_yellow(i);
		packetssl.robots_yellow[robot.robot_id()] = idCam;
		packetssl.robots_yellow[robot.robot_id() + 12] = robot.x();
		packetssl.robots_yellow[robot.robot_id() + 24] = robot.y();
		packetssl.robots_yellow[robot.robot_id() + 36] = robot.orientation();
	}
	// [End] Ball info

	emit processPacket(packetssl);
}

void MainAlg::formStatistics()
{
	QString tmp;
	QString ToStatus = "Using Matlab: PacketsPerSec = ";
	tmp.setNum(packetsPerSecond); //it is updated in other thread!!!
	ToStatus += tmp;
	ToStatus += " Total packets = ";
	tmp.setNum(totalPacketsNum); //it is updated in other thread!!!
	ToStatus += tmp;
	emit StatusMessage(ToStatus);
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
