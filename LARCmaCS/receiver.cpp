#include "receiver.h"
#include <iostream>

using namespace std;

Receiver::Receiver(){}

Receiver::~Receiver()
{
    stop();
    thread.wait(100);
    thread.terminate();
    thread.wait(100);
}

void Receiver::init()
{
    worker.moveToThread(&thread);
    cout << "Init ok" << endl;
    connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
    connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
    connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
}

void Receiver::start()
{
    thread.start();
//        cout << "thread start" << endl;
    emit wstart();
}

void Receiver::stop()
{
    emit wstop();
}
