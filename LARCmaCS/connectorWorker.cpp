#include "connectorWorker.h"
#include <QMap>

ConnectorWorker::ConnectorWorker(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mUdpSocket(this)
	, mStatisticsTimer(this)
{
//	connect(&mUdpSocket, SIGNAL(readyRead()), this, SLOT(udpProcessPendingDatagrams()));
}

void ConnectorWorker::stop()
{
	mUdpSocket.close();
}

const QString & ConnectorWorker::getGrSimIP()
{
	return grSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
	return grSimPort;
}

void ConnectorWorker::start()
{
}

void ConnectorWorker::run(int N, const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(mSharedRes->getRobotIP(N)), DefaultRobot::robotPort);
}

void ConnectorWorker::runSim(const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(grSimIP), grSimPort);
}

void ConnectorWorker::changeGrSimIP(const QString & IP)
{
	grSimIP = IP;
}

void ConnectorWorker::changeGrSimPort(unsigned short port)
{
	grSimPort = port;
}
