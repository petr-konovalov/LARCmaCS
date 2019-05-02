#include "connectorWorker.h"

#include <QMap>

ConnectorWorker::ConnectorWorker(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mUdpSocket(this)
	, mStatisticsTimer(this)
{
}

void ConnectorWorker::stop()
{
	mUdpSocket.close();
}

const QString & ConnectorWorker::getGrSimIP()
{
	return mGrSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
	return mGrSimPort;
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
	mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), mGrSimPort);
}

void ConnectorWorker::onConnectorChange(bool isSim, const QString &ip, int port)
{
	mIsSim = isSim;

	if (mIsSim) {
		mGrSimIP = ip;
		mGrSimPort = port;
	}
}
