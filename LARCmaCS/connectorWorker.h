#pragma once

#include <QUdpSocket>
#include <QMap>
#include <QTimer>

#include "constants.h"
#include "sharedRes.h"
#include "settings.h"
#include "defaultRobot.h"

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
	void start();
	void stop();
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);
	void onConnectorChange(bool isSim, const QString & ip, int port);

private:
	QString mGrSimIP;
	unsigned short mGrSimPort;
	SharedRes * mSharedRes;
	QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
	bool mIsSim;
	static const unsigned short mRobotPort = 10000;
};
