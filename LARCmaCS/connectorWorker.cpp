#include "connectorWorker.h"
#include <QMap>
#include "settings.h"

static QString configKeyForRobotNum(int n)
{
	return "robot_ip_" + QString::number(n);
}

void ConnectorWorker::start()
{
	qDebug() << "Connector worker started";
	shutdownconnector = false;
	init();
}

void ConnectorWorker::stop()
{
	shutdownconnector = true;
}

void ConnectorWorker::init()
{
	qDebug() << "Initializing connector.worker";

	connectedSockets = 0;
	connectedRobots = 0;
	//filename = "numMAC.txt";
	command.resize(6);
	command[0]='r';
	command[1]='u';
	command[2]='l';
	command[3]='e';
	command[4]=' ';


	dat[0]='<';
	dat[1]='b';
	dat[2]='r';
	dat[3]='o';
	dat[4]='a';
	dat[5]='d';
	dat[6]=' ';

	dat[10]='c';
	dat[11]='>';
	udpSocket = new QUdpSocket(this);

	connectedAllSocketsFlag = false;
	curRuleArray = new char[4 * 7]; // FIXME!

	gotPacketsNum = 0;
	Settings s;
	for(auto i = 1; i <= 12; ++i) {
		numIP[i] = s.value(configKeyForRobotNum(i),"").toString();
	}

	timer = new QTimer(this);
	connect(udpSocket,SIGNAL(readyRead()),this,SLOT(udpProcessPendingDatagrams()));
	qDebug()<<"INIT CONNECTOR OK";
}

void ConnectorWorker::receiveMacArray(QString * macArray)
{
	for (int i=0;i<12;++i) {
		qDebug()<<macArray[i];
		if (macArray[i].compare("")) { //if robot exists
			if (macArray[i].contains("bro")) //if it's arduino
				numIP[i+1] = macArray[i];
			else
				numIP[i+1] = macIP[macArray[i]]; //if it's TRIK
		}
	}
	qDebug()<<numIP;
	connectedAllSocketsFlag = true;
}

QString ConnectorWorker::getGrSimIP()
{
	return grSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
	return grSimPort;
}

void ConnectorWorker::run(int N, const QByteArray &command)
{
	udpSocket->writeDatagram(command, QHostAddress(numIP[N]), 10000);
}

void ConnectorWorker::runSim(const QByteArray &command)
{
	udpSocket->writeDatagram(command, QHostAddress(grSimIP), grSimPort);
}

void ConnectorWorker::changeGrSimIP(const QString &IP)
{
	grSimIP = IP;
}

void ConnectorWorker::changeGrSimPort(unsigned short port)
{
	grSimPort = port;
}

void ConnectorWorker::addIp(int id, QString ip)
{
	if (ip.contains(".")) {
		numIP[id] = ip;
	} else {
		 numIP[id] = "";
	}
	Settings s;
	s.setValue(configKeyForRobotNum(id), ip);
}

void ConnectorWorker::udpProcessPendingDatagrams()
{
	qDebug()<<"DATAGRAM!";
	QByteArray datagram;
	QHostAddress *robotAddress = new QHostAddress();
	while (udpSocket->hasPendingDatagrams()) {

		datagram.resize(udpSocket->pendingDatagramSize());
		udpSocket->readDatagram(datagram.data(), datagram.size(),robotAddress);
		QString str(datagram);

		QStringList lst = str.split(' ');
		if (!lst[0].compare("mac")) {
			if (!macList.contains(lst[1])) {
				macList.push_back(lst[1]);
				macIP[lst[1]] = robotAddress->toString();
				emit robotAdded(lst[1]);
			}
		}
	}
}
