#include "connector.h"

Connector::Connector()
{
	mWorker = new ConnectorWorker();
	mThread = new QThread();
}

Connector::~Connector()
{
	stop();
}

void Connector::init(SharedRes * sharedRes)
{
	mSharedRes = sharedRes;
	connect(mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(this, SIGNAL(wstop()), mWorker, SLOT(stop()));
	connect(mWorker, SIGNAL(finished()), mWorker, SLOT(deleteLater()));
	connect(mThread, SIGNAL(finished()), mThread, SLOT(deleteLater()));
	connect(mWorker, SIGNAL(finished()), mThread, SLOT(quit()));
	connect(this, SIGNAL(sendPacket(int, const QByteArray &)), mWorker, SLOT(run(int, const QByteArray &)));
	connect(this, SIGNAL(sendSimPacket(const QByteArray &)), mWorker, SLOT(runSim(const QByteArray &)));
	connect(this, SIGNAL(setGrSimIP(const QString &)), mWorker, SLOT(changeGrSimIP(const QString &)));
	connect(this, SIGNAL(setGrSimPort(unsigned short)), mWorker, SLOT(changeGrSimPort(unsigned short)));
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

void Connector::start()
{
	mThread->start();
	mThread->setPriority(QThread::HighestPriority);
}

void Connector::stop()
{
	emit wstop();
}
