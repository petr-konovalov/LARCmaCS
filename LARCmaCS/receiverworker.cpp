#include "receiverWorker.h"
#include <QtWidgets/QApplication>

ReceiverWorker::ReceiverWorker()
{
	client = new RoboCupVisionClient();
}

ReceiverWorker::~ReceiverWorker()
{
	emit clientClose();
	delete client;
}

int ReceiverWorker::getTotalPacketsNum()
{
	return client->getTotalPacketsNum();
}

int ReceiverWorker::getPacketsPerSecond()
{
	return client->getPacketsPerSecond();
}

void ReceiverWorker::newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry)
{
	emit VisionDataReady(detection, geometry);
}

void ReceiverWorker::askForSwapDataVectors()
{
	emit swapDataVectors();
}

void ReceiverWorker::start()
{
	cout << "Receiver worker start" << endl;
	connect(this, SIGNAL(clientOpen(unsigned short)), client, SLOT(open(unsigned short)));
	connect(this, SIGNAL(clientClose()), client, SLOT(close()));
	connect(client, SIGNAL(socketClosed()), this, SLOT(socketClosed()));
	connect(client, SIGNAL(socketClosed()), client, SLOT(clearOutput()));
	connect(this, SIGNAL(swapDataVectors()), client, SLOT(swapDataVectors()), Qt::DirectConnection);
	connect(client, SIGNAL(newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)),
			this, SLOT(newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)), Qt::DirectConnection);
	emit clientOpen(SSL_VISION_PORT);
}

void ReceiverWorker::socketClosed()
{
	emit clearField();
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

//void ReceiverWorker::processPacket(SSL_WrapperPacket * packet)
//{
//	int balls_n = 0;
//	SSL_DetectionBall ball;
//	SSL_DetectionFrame detection;

//	int robots_blue_n = 0;
//	int robots_yellow_n = 0;
//	SSL_DetectionRobot robot;

//	int idCam = 0;

//	int packetsNum = 0;

//	if (packet->has_geometry()) {
//		emit updatefieldGeometry(packet);
//	}
//	else if (packet->has_detection()) {
//		Time_count++;
//		packetsNum++;
//		//cout << "Num RECEIVER:" << packetsNum << endl;


//		qRegisterMetaType<PacketSSL>("PacketSSL"); // for queueing arguments between threads

//		detection = packet->detection();

//		idCam = detection.camera_id() + 1;
//		balls_n = detection.balls_size();

//		// [Start] Ball info
//		if (balls_n != 0) {
//			packetssl.balls[0] = idCam;
//			ball = detection.balls(0);
//			packetssl.balls[1] = ball.x();
//			packetssl.balls[2] = ball.y();
//		} else
//			packetssl.balls[0] = 0;

//		robots_blue_n = detection.robots_blue_size();
//		robots_yellow_n = detection.robots_yellow_size();
//		// [End] Ball info

//		// [Start] Robot info
//		for (int i = 0; i < TEAM_COUNT / 4; i++)
//			if(packetssl.robots_blue[i] == idCam)
//				packetssl.robots_blue[i] = 0;

//		for (int i = 0; i < robots_blue_n; i++) {
//			robot = detection.robots_blue(i);
//			packetssl.robots_blue[robot.robot_id()] = idCam;
//			packetssl.robots_blue[robot.robot_id() + 12] = robot.x();
//			packetssl.robots_blue[robot.robot_id() + 24] = robot.y();
//			packetssl.robots_blue[robot.robot_id() + 36] = robot.orientation();
//		}

//		for (int i = 0; i < TEAM_COUNT / 4; i++)
//			if(packetssl.robots_yellow[i] == idCam)
//				packetssl.robots_yellow[i] = 0;

//		for (int i = 0; i < robots_yellow_n; i++) {
//			robot = detection.robots_yellow(i);
//			packetssl.robots_yellow[robot.robot_id()] = idCam;
//			packetssl.robots_yellow[robot.robot_id() + 12] = robot.x();
//			packetssl.robots_yellow[robot.robot_id() + 24] = robot.y();
//			packetssl.robots_yellow[robot.robot_id() + 36] = robot.orientation();
//		}
//		// [End] Ball info
//		QApplication::processEvents();
//		//emit activateGUI(packet);
//	}

//	if (clock()-timer_m>CLOCKS_PER_SEC) {
//		timer_m=clock();
//		QString temp;
//		QString ToStatus="FPS=";
//		temp.setNum(Time_count);
//		ToStatus=ToStatus+temp;
//		ToStatus=ToStatus+"; Count=";
//		temp.setNum(packetsNum);
//		ToStatus=ToStatus+temp;
//		Time_count=0;
//		emit UpdateSSLFPS(ToStatus);
//	}
//	QApplication::processEvents();
//}
