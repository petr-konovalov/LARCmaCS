#pragma once

#include <QObject>
#include <QFile>
#include <QUdpSocket>
#include <QtCore>
#include <QMap>

using std::map;

class ConnectorWorker : public QObject
{
Q_OBJECT
public:
	ConnectorWorker(){}

	const QString & getGrSimIP();
	unsigned short getGrSimPort();

	int shutdownconnector;
	char *curRuleArray;

	QString filename;
	QFile *ipFile;
	QUdpSocket *udpSocket;
	map<int const, QString> robotAddrMap;
	int connectedAllSocketsFlag;
	QMap<int,QString> numIP;
	QMap<QString,QString> macIP;

	QList<QString> macList;

	int gotPacketsNum;
	QTimer* timer;

	int connectedSockets;
	int connectedRobots;
	QByteArray command;

	char dat[12];

private:
	QString grSimIP = "127.0.0.1";
	unsigned short grSimPort = 20011;

signals:
	void robotAdded(QString);
	void sendMacs(QList<QString>);
	void sendPortList(QStringList);

public slots:
	void init();
	void start();
	void stop();
	void changeGrSimIP(const QString & IP);
	void changeGrSimPort(unsigned short port);
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);
	void udpProcessPendingDatagrams();
	void addIp(int id, QString ip);
};
