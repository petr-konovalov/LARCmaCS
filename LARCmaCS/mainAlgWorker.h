#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include "packetSSL.h"
#include "mlData.h"

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

public slots:
	void start();
	void formStatistics();
	void updatePauseState();
	void stop();
	void processPacket(const QSharedPointer<PacketSSL> & packetssl);
	void Pause();
	void runMatlab();
	void stop_matlab();
	void run();
	void EvalString(const QString & s);
	void changeBallStatus(bool ballStatus);
	void changeConnector(bool isSim, const QString &, int);

private:
	void init();
	bool mIsSimEnabledFlag = 0;
	char m_buffer[256]; // matlab buffer
	MlData fmldata;
	bool fmtlab;
	bool mShutdownFlag;
	bool pause;
	bool mIsPause;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	QTimer mStatisticsTimer;
	QSharedPointer<PacketSSL> mPacketSSL;
	double mIsBallInside;
};
