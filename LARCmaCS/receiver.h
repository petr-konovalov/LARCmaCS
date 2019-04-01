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
	QTimer mStatisticTimer;
	QTimer mDisplayTimer;
	bool mDisplayFlag = false;

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
	void clearScene();
	void ChangeSimulatorMode(bool mode);
	void setDisplayFlag();
	void VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);

signals:
	void wstart();
	void updateSimulatorMode(bool mode);
	void clearField();
	void UpdateSSLFPS(QString status);
	void sendDataToDisplay(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);
	void sendDataToMainAlg(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);
	void wstop();
};
