#include "receiver.h"
#include <iostream>

using namespace std;

Receiver::Receiver(){}

Receiver::~Receiver()
{
	stop();
	thread.wait(100);
	thread.terminate();
	thread.wait(100);
}

void Receiver::init()
{
	connect(this, SIGNAL(initWorker()), &worker, SLOT(init()));
	worker.moveToThread(&thread);
	emit initWorker();
	cout << "Init ok" << endl;
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
	connect(&worker, SIGNAL(clearField()), this, SLOT(clearScene()));
	connect(&worker, SIGNAL(UpdateSSLFPS(QString)), this, SLOT(sendStatistics(QString)));
	connect(this, SIGNAL(updateSimulatorMode(bool)), &worker, SLOT(ChangeSimulatorMode(bool)));
}

void Receiver::sendStatistics(QString statistics)
{
	emit UpdateSSLFPS(statistics);
}

QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > Receiver::getVisionData()
{
	return worker.getVisionData();
}

void Receiver::swapDataVectors()
{
	worker.swapDataVectors();
}

void Receiver::setDisplayFlag()
{
	mDisplayFlag = true;
}

void Receiver::ChangeSimulatorMode(bool mode)
{
	emit updateSimulatorMode(mode);
}

void Receiver::clearScene()
{
	emit clearField();
}

void Receiver::start()
{
	thread.start();
	emit wstart();
}

void Receiver::stop()
{
	emit wstop();
}
