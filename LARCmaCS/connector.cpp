#include "connector.h"

#include <stdlib.h>
#include <iostream>
#include <QMap>

Connector::Connector() {}

Connector::~Connector()
{
	stop();
	thread.wait(100);
	thread.terminate();
	thread.wait(100);
}

void Connector::init()
{
	worker.moveToThread(&thread);
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
}

void Connector::start()
{
	thread.start();
	thread.setPriority(QThread::HighestPriority);
	emit wstart();
}

void Connector::stop()
{
	emit wstop();
}
