#include "connectorWorker.h"
#include <QMap>

static QString configKeyForRobotNum(int n)
{
	return "robot_ip_" + QString::number(n);
}

void ConnectorWorker::start()
{
	init();
}

void ConnectorWorker::stop()
{
	mUdpSocket->close();
	emit finished();
}

void ConnectorWorker::init()
{
	Settings settings;
	mUdpSocket = new QUdpSocket();
	for (auto i = 1; i <= 12; i++) {
		mIPRobotList[i] = settings.value(configKeyForRobotNum(i), "").toString();
	}
	mStatisticsTimer = new QTimer();
	connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(udpProcessPendingDatagrams()));
}

const QString & ConnectorWorker::getGrSimIP()
{
	return grSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
	return grSimPort;
}

void ConnectorWorker::run(int N, const QByteArray & command)
{
	mUdpSocket->writeDatagram(command, QHostAddress(mIPRobotList[N]), DefaultRobot::robotPort);
}

void ConnectorWorker::runSim(const QByteArray & command)
{
	mUdpSocket->writeDatagram(command, QHostAddress(grSimIP), grSimPort);
}

void ConnectorWorker::changeGrSimIP(const QString & IP)
{
	grSimIP = IP;
}

void ConnectorWorker::changeGrSimPort(unsigned short port)
{
	grSimPort = port;
}

QMap<int, QString> & ConnectorWorker::getIPList()
{
	return mIPRobotList;
}

void ConnectorWorker::addIP(int id, const QString & ip)
{
	if (ip.contains(".")) {
		mIPRobotList[id] = ip;
	} else {
		mIPRobotList[id] = "";
	}
	Settings settings;
	settings.setValue(configKeyForRobotNum(id), ip);
}

void ConnectorWorker::udpProcessPendingDatagrams()
{
	QByteArray datagram;
	QHostAddress * robotAddress = new QHostAddress();
	while (mUdpSocket->hasPendingDatagrams()) {
		datagram.resize(mUdpSocket->pendingDatagramSize());
		mUdpSocket->readDatagram(datagram.data(), datagram.size(), robotAddress);
	}
}
