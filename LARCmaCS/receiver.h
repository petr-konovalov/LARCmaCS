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
	QTimer mDisplayTimer;
	bool mDisplayFlag = false;

public:
	QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket> > > getVisionData();
	ReceiverWorker worker;
	QThread thread;

	Receiver();
	~Receiver();

	void init();
	void start();
	void stop();

public slots:
	void swapDataVectors();
	void sendStatistics(QString statistics);
	void clearScene();
	void ChangeSimulatorMode(bool mode);
	void setDisplayFlag();

signals:
	void wstart();
	void initWorker();
	void updateSimulatorMode(bool mode);
	void clearField();
	void UpdateSSLFPS(QString status);
	void wstop();
};
