#include "connector.h"

Connector::Connector(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mWorker(new ConnectorWorker(sharedRes))
{
	mWorker->moveToThread(&mThread);
	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(this, SIGNAL(sendPacket(int, const QByteArray &)), mWorker, SLOT(run(int, const QByteArray &)));
	connect(this, SIGNAL(sendSimPacket(const QByteArray &)), mWorker, SLOT(runSim(const QByteArray &)));
	connect(this, SIGNAL(setGrSimIP(const QString &)), mWorker, SLOT(changeGrSimIP(const QString &)));
	connect(this, SIGNAL(setGrSimPort(unsigned short)), mWorker, SLOT(changeGrSimPort(unsigned short)));

	mThread.start();
}

Connector::~Connector()
{
	mThread.quit();
	mThread.wait();
}

const QString & Connector::getGrSimIP()
{
	return mWorker->getGrSimIP();
}

unsigned short Connector::getGrSimPort()
{
	return mWorker->getGrSimPort();
}

void Connector::changeGrSimIP(const QString & IP)
{
	emit setGrSimIP(IP);
}

void Connector::changeGrSimPort(unsigned short port)
{
	emit setGrSimPort(port);
}

void Connector::run(int N, const QByteArray & command)
{
	emit sendPacket(N, command);
}

void Connector::runSim(const QByteArray & command)
{
	emit sendSimPacket(command);
}
