#pragma once

#include <QObject>
#include <QFile>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

#include <QtCore>
#include <QtNetwork>
#include <QTimer>
#include <QMap>

#include "connectorWorker.h"

using std::map;
using std::vector;
#include <Set>

struct Connector : QObject
{
    Q_OBJECT

public:
    ConnectorWorker worker;
    QThread thread;

    explicit Connector();
    ~Connector();

    void init();

    void start();

    void stop();

signals:
    void wstart();
    void wstop();
};
