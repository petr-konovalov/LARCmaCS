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
#include "receiver.h"
#include "messages_robocup_ssl_wrapper.pb.h"

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct MainAlg : public QObject
{
	Q_OBJECT
private:
	QSharedPointer<PacketSSL> mPacketSSL;
	Receiver * mReceiver;
	MainAlgWorker worker;
	QThread thread;
public:
	bool getIsSimEnabledFlag();
	explicit MainAlg();
	~MainAlg();
	void init(Receiver * receiver);
	void start();
	void stop();

public slots:
	void EvalString(QString s);
	void moveToConnector(int N, const QByteArray & command);
	void moveToSimConnector(const QByteArray & command);
	void receivePauseState(QString state);
	void setEnableSimFlag(bool flag);
	void changeBallStatus(bool status);
	void loadVisionData();
	void sendStatistics(QString statistics);

signals:
	void updateBallStatus(bool status);
	void MLEvalString(QString s);
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void updateEnableSimFlag(bool flag);
	void StatusMessage(QString status);
	void UpdatePauseState(QString state);
	void wstart();
	void wstop();
};
