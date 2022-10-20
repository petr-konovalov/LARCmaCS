#pragma once

#include <QUdpSocket>
#include <QMap>
#include <QTimer>
#include <QReadWriteLock>

#include "constants.h"
#include "sharedRes.h"
#include "settings.h"
#include "defaultRobot.h"
#include "engineInterface.h"

using std::map;

class ConnectorWorker : public QObject
{
	Q_OBJECT

public:
    ConnectorWorker(SharedRes * sharedRes);
    ~ConnectorWorker();
	const QString & getGrSimIP();
	unsigned short getGrSimPort();
	unsigned short getRobotPort();

public slots:
    void onConnectorChange(bool isSim, const QString & ip, int port, int portYellow, const QString &netInterface);
	void run(int N, const QByteArray & command);
    void runSim(const QByteArray & command, bool isYellow);
    void start();

private:
	QString mGrSimIP;
	unsigned short mGrSimPort;
    unsigned short mGrSimPortYellow;
	SharedRes * mSharedRes;
	QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
	static const QString robotBoxIP;
    bool mShutdownFlag {false };

	bool mIsSim { false };
    static const unsigned short mRobotPort = 10000;
	bool mIsPause { false };
};
