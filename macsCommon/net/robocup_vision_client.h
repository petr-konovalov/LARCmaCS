// Copyright 2019 Dmitrii Iarosh

#pragma once

#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include "messages_robocup_ssl_wrapper.pb.h"
using namespace std;

static const QString visionIP = QStringLiteral("224.5.23.2");

class RoboCupVisionClient : public QObject {
    Q_OBJECT
protected:
    static const int MaxDataGramSize = 65536;
    QUdpSocket socket;
    QHostAddress groupAddress;
    QMutex mutex;
    SSL_WrapperPacket * outputPacket;
    unsigned short _port;
    bool newPacket = false;
    SSL_WrapperPacket * packet;
public:
    RoboCupVisionClient(unsigned short port);
    ~RoboCupVisionClient();
    bool receive(SSL_WrapperPacket ** packet);

private slots:
    void processPendingDatagrams();
    bool open();
    void close();

};
