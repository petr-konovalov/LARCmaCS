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
	void changeGrSimIP(const QString & IP);
	void changeGrSimPort(unsigned short port);
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);

private:
	QString grSimIP = "127.0.0.1";
	unsigned short grSimPort = 20011;
	SharedRes * mSharedRes;
	QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
};
