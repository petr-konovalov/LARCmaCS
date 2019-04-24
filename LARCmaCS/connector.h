#pragma once

#include <QObject>
#include <QThread>
#include <QtCore>
#include <QtNetwork>
#include "sharedRes.h"

#include "connectorWorker.h"

class Connector : public QObject
{
	Q_OBJECT
private:
	ConnectorWorker * mWorker;
	SharedRes * mSharedRes;
	QThread * mThread;
public:
	explicit Connector();
	~Connector();
	static const unsigned short robotPort = 10000;
	void init(SharedRes * sharedRes);
	void start();
	void stop();
	const QString & getGrSimIP();
	unsigned short getGrSimPort();

signals:
	void wstop();
	void sendPacket(int N, const QByteArray & command);
	void sendSimPacket(const QByteArray & command);
	void setGrSimIP(const QString & IP);
	void setGrSimPort(unsigned short port);

public slots:
	void changeGrSimIP(const QString & IP);
	void changeGrSimPort(unsigned short port);
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);
};
