#pragma once

#include <QObject>
#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "grSimRobot.h"
#include "defaultRobot.h"
#include "constants.h"

using namespace std;

class MainAlgWorker : public QObject
{
	Q_OBJECT

public:
	MainAlgWorker();
	void setPacketSSL(const QSharedPointer<PacketSSL> & packetSSL);
	bool getIsSimEnabledFlag();
	~MainAlgWorker();

signals:
	void finished();
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void newPauseState(const QString & state);
	void sendStatistics(const QString & statistics);
	void getDataFromReceiver();
	void toMatlabConsole(const QString & str);

public slots:
	void start();
	void setMatlabDebugFrequency(int frequency);
	void formStatistics();
	void updatePauseState();
	void stop();
	void setEnableSimFlag(bool flag);
	void processPacket(const QSharedPointer<PacketSSL> & packetssl);
	void Pause();
	void runMatlab();
	void stop_matlab();
	void run();
	void evalString(const QString & s);
	void changeBallStatus(bool ballStatus);

private:
	void init();
	bool mIsSimEnabledFlag = false;
	char mMatlabOutputBuffer[Constants::matlabOutputBufferSize];
	MlData fmldata;
	bool fmtlab;
	int mFrequency = 1;
	bool mShutdownFlag;
	bool pause;
	Client client;
	bool mIsPause;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	QTimer mStatisticsTimer;
	QSharedPointer<PacketSSL> mPacketSSL;
	double mIsBallInside;
};
