#pragma once

#include <QObject>
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
	const QString & getGrSimIP();
	unsigned short getGrSimPort();

public slots:
	void start();
	void stop();
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);
	void onConnectorChange(bool isSim, const QString & ip, int port);

private:
	QString mGrSimIP = "127.0.0.1";
	unsigned short mGrSimPort = 20011;
	SharedRes * mSharedRes;
	QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
	bool mIsSim;
};
