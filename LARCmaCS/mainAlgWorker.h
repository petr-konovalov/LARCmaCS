#pragma once

#include <QObject>
#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "grsimtransforms.h"
#include "constants.h"

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

class MainAlgWorker : public QObject
{
	Q_OBJECT
private:
	bool Send2BT[Constants::maxNumOfRobots];
	Client client;
	bool isPause;
	double mIsBallInside;

public:
	MainAlgWorker();
	bool getIsSimEnabledFlag();
	~MainAlgWorker();

signals:
	void sendToConnector(int N, QByteArray command);
	void sendToSimConnector(QByteArray command);
	void newPauseState(QString state);
	void startIteration();
	void getDataFromReceiver();

public slots:
	void start();
	void updatePauseState();
	void Send2BTChangeit(bool *send2BT_);
	void stop();
	void setEnableSimFlag(bool flag);
	void processPacket(PacketSSL & packetssl);
	void Pause();
	void run_matlab();
	void stop_matlab();
	void getPacketFromReceiver();
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
