#pragma once

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include "packetSSL.h"
#include "robocup_ssl_client.h"
#include <iostream>
#include "receiverWorker.h"

using namespace std;

#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct Receiver : public QObject
{
    Q_OBJECT
public:
    ReceiverWorker worker;
    QThread thread;

    Receiver() {}
    ~Receiver()
    {
        stop();
        thread.wait(100);
        thread.terminate();
        thread.wait(100);
    }

    void init()
    {

        worker.moveToThread(&thread);
        cout << "Init ok" << endl;
        connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
        connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
        connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
    }

    void start()
    {
        thread.start();
//        cout << "thread start" << endl;
        emit wstart();
    }

    void stop() { emit wstop(); }

signals:
    void wstart();
    void wstop();
};
