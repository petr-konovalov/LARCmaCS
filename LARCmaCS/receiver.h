#pragma once

#include <iostream>
#include <QObject>
#include <QThread>
#include <QSharedPointer>

#include "packetSSL.h"

#include "receiverWorker.h"

using namespace std;

#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct Receiver : public QObject
{
	Q_OBJECT
private:
	QTimer statisticTimer;

public:
	ReceiverWorker worker;
	QThread thread;

	Receiver();
	~Receiver();

	void init();
	void start();
	void stop();

public slots:
	void receiveRequestFromMainAlg();
	void formStatistics();
	void VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);

signals:
	void wstart();
	void UpdateSSLFPS(QString status);
	void sendDataToMainAlg(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);
	void wstop();
};
