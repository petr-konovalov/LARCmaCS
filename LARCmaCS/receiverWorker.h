#pragma once

#include <iostream>
#include <QObject>

#include "packetSSL.h"
#include "robocup_vision_client.h"

#include <time.h>

#define SSL_VISION_PORT 10006
#define SIM_VISION_PORT 10020

class ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();
	~ReceiverWorker();
	int getTotalPacketsNum();
	int getPacketsPerSecond();
	void askForSwapDataVectors();

public slots:
	void start();
	void ChangeSimulatorMode(bool flag);
	void socketClosed();
	void newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);

signals:
	void clientOpen(unsigned short port);
	void clientClose();
	void swapDataVectors();
	void clearField();
	void updatefieldGeometry(SSL_WrapperPacket * packet);
	void UpdateSSLFPS(QString message);
	void VisionDataReady(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);

private:
	RoboCupVisionClient * client;
	bool isSimEnabledFlag = false;
};
