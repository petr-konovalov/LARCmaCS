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
	statisticTimer.setInterval(1000);
	connect(&statisticTimer, SIGNAL(timeout()), this, SLOT(formStatistics()));
	statisticTimer.start();
}

void Receiver::receiveRequestFromMainAlg()
{
	worker.askForSwapDataVectors();
}

void Receiver::VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry)
{
	emit sendDataToMainAlg(detection, geometry);
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
	emit wstop();
}
