#pragma once

#include <iostream>
#include <QObject>

#include "packetSSL.h"
#include "robocup_ssl_client.h"
#include "robocup_grsim_client.h"

#include <time.h>

struct ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();

public slots:
	void MainAlgFree();
	void start();
	void stop();

public slots:
	void ChangeMaxPacketFrequencyMod(bool state);
	void ChangeSimulatorMode(bool flag);

public:
	SSL_DetectionFrame detection;

	SSL_GeometryFieldSize fieldsize;

signals:
	void activateGUI();
	void clientOpen();
	void clientClose();
	void clearField();
	void activateMA(PacketSSL packetssl);
	void updatefieldGeometry();
	void UpdateSSLFPS(QString message);

private:
	PacketSSL packetssl;
	clock_t timer_m;
	int Time_count;

private:
	void run();
	RoboCupSSLClient client;
	RoboCupGrSimClient simClient;
	SSL_WrapperPacket * packet;
	bool isSimEnabledFlag = 0;
	bool enableSimFlag = 0;
	bool NewPacket;
	bool shutdownread;
	bool mainalgisfree;
	bool MaxPacketFrequencyMod;
};
