#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QMap>
#include <QTimer>
#include "constants.h"
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
	QMap<int, QString> & getIPList();

private:
	QString grSimIP = "127.0.0.1";
	unsigned short grSimPort = 20011;
	QUdpSocket * mUdpSocket;
	QMap<int, QString> mIPRobotList;
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
	void udpProcessPendingDatagrams();
	void addIP(int id, const QString & ip);
};
