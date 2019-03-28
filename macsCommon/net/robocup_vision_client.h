// Copyright 2019 Dmitrii Iarosh

#pragma once

#define NUM_OF_CAMERAS 4
#define UPDATE_INTERVAL 33 //30FPS
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QTimer>
#include "messages_robocup_ssl_wrapper.pb.h"
using namespace std;

static const QString visionIP = QStringLiteral("224.5.23.2");

class RoboCupVisionClient : public QObject {
    Q_OBJECT
private:
	QTimer * timer;
protected:
    static const int MaxDataGramSize = 65536;
    QUdpSocket socket;
    QHostAddress groupAddress;
    QMutex mutex;
	SSL_WrapperPacket * outputPacket[NUM_OF_CAMERAS];
    unsigned short _port;
	bool newPacket[NUM_OF_CAMERAS];
	SSL_WrapperPacket * inputPacket;
public:
    RoboCupVisionClient(unsigned short port);
    ~RoboCupVisionClient();

private slots:
    void processPendingDatagrams();
	bool open();
	void processFrame();
    void close();
signals:
	void processPacket(SSL_WrapperPacket * packet);
};
