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
	worker.moveToThread(&thread);
	cout << "Init ok" << endl;
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(&worker, SIGNAL(VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)), this, SLOT(VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)), Qt::DirectConnection);
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
	mStatisticTimer.setInterval(1000);
	mDisplayTimer.setInterval(33); //30 FPS
	connect(&mStatisticTimer, SIGNAL(timeout()), this, SLOT(formStatistics()));
	connect(worker.getClient(), SIGNAL(clearField()), this, SLOT(clearScene()));
	connect(&mDisplayTimer, SIGNAL(timeout()), this, SLOT(setDisplayFlag()));
	connect(this, SIGNAL(updateSimulatorMode(bool)), &worker, SLOT(ChangeSimulatorMode(bool)));
	mStatisticTimer.start();
	mDisplayTimer.start();
}

void Receiver::receiveRequestFromMainAlg()
{
	worker.askForSwapDataVectors();
}

void Receiver::setDisplayFlag()
{
	mDisplayFlag = true;
}

void Receiver::VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry)
{
	emit sendDataToMainAlg(detection, geometry);
	if (mDisplayFlag) {
		emit sendDataToDisplay(detection, geometry);
		mDisplayFlag = false;
	}
}

void Receiver::ChangeSimulatorMode(bool mode)
{
	emit updateSimulatorMode(mode);
}

void Receiver::clearScene()
{
	emit clearField();
}

void Receiver::formStatistics()
{
	QString tmp;
	QString ToStatus = "FPS = ";
	tmp.setNum(worker.getPacketsPerSecond());
	ToStatus += tmp;
	ToStatus += "; Total Packets = ";
	tmp.setNum(worker.getTotalPacketsNum());
	ToStatus += tmp;
	emit UpdateSSLFPS(ToStatus);
}

void Receiver::start()
{
	thread.start();
//        cout << "thread start" << endl;
	emit wstart();
}

void Receiver::stop()
{
	mDisplayTimer.stop();
	mStatisticTimer.stop();
	emit wstop();
}
