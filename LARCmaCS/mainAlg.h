#pragma once

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include <QFile>
#include <iostream>

#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "mainAlgWorker.h"
#include "messages_robocup_ssl_wrapper.pb.h"

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct MainAlg : public QObject
{
	Q_OBJECT
private:
	QSharedPointer<PacketSSL> mPacketSSL;
	volatile int totalPacketsNum = 0;
	volatile int packetsPerSecond = 0;
	QTimer statisticTimer;
public:
	MainAlgWorker worker;
	QThread thread;

	explicit MainAlg();
	~MainAlg();

	void init();
	void start();
	void stop();

public slots:
	void getDataFromReceiver();
	void formStatistics();
	void receivePauseState(QString state);
	void receiveVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);

signals:
	void newIteration();
	void askReceiverForData();
	void StatusMessage(QString status);
	void UpdatePauseState(QString state);
	void processPacket(PacketSSL & packetssl);
	void wstart();
	void wstop();
};
