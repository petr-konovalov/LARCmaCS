#pragma once

#include <iostream>
#include <QObject>
#include <QThread>
#include <QSharedPointer>

#include "packetSSL.h"
#include "robocup_ssl_client.h"
#include "receiverWorker.h"

using namespace std;

#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct Receiver : public QObject
{
    Q_OBJECT
public:
    ReceiverWorker worker;
    QThread thread;

    Receiver();
    ~Receiver();

    void init();

    void start();

    void stop();

signals:
    void wstart();
    void wstop();
};
