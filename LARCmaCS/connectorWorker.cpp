#include "connectorWorker.h"
#include <QMap>

void ConnectorWorker::start()
{
	init();
}

void ConnectorWorker::stop()
{
	mUdpSocket->close();
	emit finished();
}

void ConnectorWorker::init()
{
	Settings settings;
	mUdpSocket = new QUdpSocket();
	mStatisticsTimer = new QTimer();
	connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(udpProcessPendingDatagrams()));
}

const QString & ConnectorWorker::getGrSimIP()
{
	return grSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
	return grSimPort;
}

void ConnectorWorker::run(int N, const QByteArray & command)
{
	mUdpSocket->writeDatagram(command, QHostAddress(sharedRes->getRobotIP(N)), DefaultRobot::robotPort);
}

void ConnectorWorker::runSim(const QByteArray & command)
{
	mUdpSocket->writeDatagram(command, QHostAddress(grSimIP), grSimPort);
}

void ConnectorWorker::changeGrSimIP(const QString & IP)
{
	grSimIP = IP;
}

void ConnectorWorker::changeGrSimPort(unsigned short port)
{
	grSimPort = port;
}
