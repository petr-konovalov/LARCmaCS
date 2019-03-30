#include "receiverWorker.h"
#include <QtWidgets/QApplication>

ReceiverWorker::ReceiverWorker()
{
	mainalgisfree=true;
	timer_m=clock();
	Time_count=0;
	client = new RoboCupVisionClient();
}

ReceiverWorker::~ReceiverWorker()
{
	emit clientClose();
	delete client;
}

void ReceiverWorker::MainAlgFree()
{
	if ((NewPacket) && (MaxPacketFrequencyMod)) {
		mainalgisfree=false;
		NewPacket=false;
		emit activateMA(packetssl);
	} else
		mainalgisfree=true;
}

void ReceiverWorker::start()
{
	shutdownread = false;
	mainalgisfree = true;
	NewPacket=false;
	MaxPacketFrequencyMod=false;
	cout << "Receiver worker start" << endl;
	connect(this, SIGNAL(clientOpen(unsigned short)), client, SLOT(open(unsigned short)));
	connect(this, SIGNAL(clientClose()), client, SLOT(close()));
	connect(client, SIGNAL(processPacket(SSL_WrapperPacket *)), this, SLOT(processPacket(SSL_WrapperPacket *)));
	connect(client, SIGNAL(socketClosed()), this, SLOT(socketClosed()));
	connect(client, SIGNAL(socketClosed()), client, SLOT(clearOutput()));
	emit clientOpen(SSL_VISION_PORT);
}

void ReceiverWorker::stop()
{
	shutdownread = true;
}

void ReceiverWorker::socketClosed()
{
	emit clearField();
}

void ReceiverWorker::ChangeMaxPacketFrequencyMod(bool state)
{
	MaxPacketFrequencyMod = state;
	cout << "MaxPacketFrequencyMod = " << state << endl;
}

void ReceiverWorker::ChangeSimulatorMode(bool flag)
{
	if (flag != isSimEnabledFlag) {
		isSimEnabledFlag = flag;
		if (isSimEnabledFlag) {
			emit clientClose();
			emit clientOpen(SIM_VISION_PORT);
		} else {
			emit clientClose();
			emit clientOpen(SSL_VISION_PORT);
		}
	}
}

void ReceiverWorker::processPacket(SSL_WrapperPacket * packet)
{
	int balls_n = 0;
	SSL_DetectionBall ball;
	SSL_DetectionFrame detection;

	int robots_blue_n = 0;
	int robots_yellow_n = 0;
	SSL_DetectionRobot robot;

	int idCam = 0;

	int packetsNum = 0;

	if (packet->has_geometry()) {
		emit updatefieldGeometry(packet);
	}
	else if (packet->has_detection()) {
		Time_count++;
		packetsNum++;
		//cout << "Num RECEIVER:" << packetsNum << endl;


		qRegisterMetaType<PacketSSL>("PacketSSL"); // for queueing arguments between threads

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
		QApplication::processEvents();
		if (mainalgisfree) {
			mainalgisfree=false;
			NewPacket=false;
			emit activateMA(packetssl);
			emit activateGUI(packet);
		} else {
			NewPacket=true;
			//if (MaxPacketFrequencyMod) //why this was an option? maybe we lost smth important
				emit activateGUI(packet);
		}
	}

	if (clock()-timer_m>CLOCKS_PER_SEC) {
		timer_m=clock();
		QString temp;
		QString ToStatus="FPS=";
		temp.setNum(Time_count);
		ToStatus=ToStatus+temp;
		ToStatus=ToStatus+"; Count=";
		temp.setNum(packetsNum);
		ToStatus=ToStatus+temp;
		Time_count=0;
		emit UpdateSSLFPS(ToStatus);
	}
	QApplication::processEvents();
}
