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
	ConnectorWorker(){}
	const QString & getGrSimIP();
	unsigned short getGrSimPort();

private:
	QString grSimIP = "127.0.0.1";
	unsigned short grSimPort = 20011;
	QUdpSocket * mUdpSocket;
	SharedRes * sharedRes;
	QTimer * mStatisticsTimer;
signals:
	void finished();

public slots:
	void init();
	void start();
	void stop();
	void changeGrSimIP(const QString & IP);
	void changeGrSimPort(unsigned short port);
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);
};
