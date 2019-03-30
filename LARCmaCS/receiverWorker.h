#pragma once

#include <iostream>
#include <QObject>

#include "packetSSL.h"
#include "robocup_vision_client.h"

#include <time.h>

#define SSL_VISION_PORT 10006
#define SIM_VISION_PORT 10020

struct ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();
	~ReceiverWorker();

public slots:
	void MainAlgFree();
	void start();
	void stop();
	void ChangeMaxPacketFrequencyMod(bool state);
	void processPacket(SSL_WrapperPacket * packet);
	void ChangeSimulatorMode(bool flag);
	void socketClosed();

signals:
	void activateGUI(SSL_WrapperPacket * packet);
	void clientOpen(unsigned short port);
	void clientClose();
	void clearField();
	void activateMA(PacketSSL packetssl);
	void updatefieldGeometry(SSL_WrapperPacket * packet);
	void UpdateSSLFPS(QString message);

private:
	PacketSSL packetssl;
	clock_t timer_m;
	int Time_count;
	RoboCupVisionClient * client;
	bool isSimEnabledFlag = false;
	bool NewPacket;
	bool shutdownread;
	bool mainalgisfree;
	bool MaxPacketFrequencyMod;
};
