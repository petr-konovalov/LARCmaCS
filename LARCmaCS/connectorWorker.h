#pragma once

#include <QObject>
#include <QFile>
#include <QUdpSocket>
#include <QtCore>
#include <QMap>

using std::map;

struct ConnectorWorker : QObject
{
Q_OBJECT
public:
	ConnectorWorker(){}

	int shutdownconnector;
	char *curRuleArray;

	QString filename;
	QFile *ipFile;
	QUdpSocket *udpSocket;
	map<int const, QString> robotAddrMap;
	int connectedAllSocketsFlag;
	QMap<int,QString> numIP;
	QString grSimIP = "127.0.0.1";
	unsigned short grSimPort = 20011;
	QMap<QString,QString> macIP;

	QList<QString> macList;

	int gotPacketsNum;
	QTimer* timer;

	int connectedSockets;
	int connectedRobots;
	QByteArray command;

	char dat[12];

signals:
	void receivedDatagram(QByteArray datagram);
	void receivedAddresses(QByteArray addressDatagram);
	void gotStringAddresses(QByteArray stringAddressDatagram);
	void allNeededRobotsEnabled();
	void allTasksDone();
	void robotAdded(QString);
	void sendMacs(QList<QString>);
	void sendPortList(QStringList);

public slots:
	void init();
	void start();
	void stop();
	void changeGrSimIP(QString IP);
	void changeGrSimPort(unsigned short port);
	void run(int N, QByteArray command);
	void runSim(QByteArray command);
	void udpProcessPendingDatagrams();
	void receiveMacArray(QString*);
	void addIp(int id, QString ip);
};
