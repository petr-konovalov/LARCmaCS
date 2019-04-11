#pragma once

#include <QObject>
#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "grSimRobot.h"
#include "defaultRobot.h"
#include "constants.h"

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

class MainAlgWorker : public QObject
{
	Q_OBJECT
private:
	Client client;
	bool isPause;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	QTimer mStatisticsTimer;
	QSharedPointer<PacketSSL> mPacketSSL;
	double mIsBallInside;

public:
	MainAlgWorker();
	void setPacketSSL(QSharedPointer<PacketSSL> packetSSL);
	bool getIsSimEnabledFlag();
	~MainAlgWorker();

signals:
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void newPauseState(QString state);
	void sendStatistics(QString statistics);
	void getDataFromReceiver();

public slots:
	void start();
	void formStatistics();
	void updatePauseState();
	void stop();
	void setEnableSimFlag(bool flag);
	void processPacket(QSharedPointer<PacketSSL> packetssl);
	void Pause();
	void run_matlab();
	void stop_matlab();
	void run();
	void EvalString(QString s);
	void changeBallStatus(bool ballStatus);

private:
	void init();
	bool isSimEnabledFlag = 0;
	char m_buffer[256]; // matlab buffer
	MlData fmldata;
	bool fmtlab;
	bool shutdowncomp;
	bool pause;
};
