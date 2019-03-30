#pragma once

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include <QFile>
#include <iostream>

#include "packetSSL.h"
#include "mlData.h"
#include "client.h"
#include "mainAlgWorker.h"

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct MainAlg : public QObject
{
	Q_OBJECT

public:
	MainAlgWorker worker;
	QThread thread;

	explicit MainAlg();
	~MainAlg();

	void init();
	void start();
	void stop();

signals:
	void wstart();
	void wstop();
};
