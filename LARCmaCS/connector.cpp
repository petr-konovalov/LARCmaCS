#include "connector.h"

#include <QMap>

Connector::Connector(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mUdpSocket(this)
	, mStatisticsTimer(this)
{}

Connector::~Connector()
{}

const QString & Connector::getGrSimIP()
{
	return mGrSimIP;
}

unsigned short Connector::getGrSimPort()
{
	return mGrSimPort;
}

unsigned short Connector::getRobotPort()
{
	return mRobotPort;
}

void Connector::run(int N, const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(mSharedRes->getRobotIP(N)), DefaultRobot::robotPort);
}

void Connector::runSim(const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), mGrSimPort);
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port)
{
	mIsSim = isSim;

	if (mIsSim) {
		mGrSimIP = ip;
		mGrSimPort = port;
	}
}
