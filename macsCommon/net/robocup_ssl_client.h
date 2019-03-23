// Copyright 2019 Dmitrii Iarosh

#pragma once

#include <string>
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "messages_robocup_ssl_refbox_log.pb.h"
using namespace std;

class RoboCupSSLClient : public QObject {
	Q_OBJECT
protected:
	static const int MaxDataGramSize = 65536;
	char * in_buffer;
	QUdpSocket socket;
	QHostAddress groupAddress;
	QMutex mutex;
	SSL_WrapperPacket * outputPacket;
	unsigned short _port;
	bool newPacket = false;
	SSL_WrapperPacket * packet;
public:
#ifdef OLD_SSL_PROTO
	RoboCupSSLClient(unsigned short port = 10002);
#else
	RoboCupSSLClient(unsigned short port = 10006);
#endif
	~RoboCupSSLClient();
	bool receive(SSL_WrapperPacket ** packet);

private slots:
	void processPendingDatagrams();
	bool open();
	void close();

};

