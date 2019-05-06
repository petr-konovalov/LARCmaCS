#include "mainAlgWorker.h"

#include <QDebug>
#include <QApplication>

#include "message.h"
#include "constants.h"
#include "matlabEngine.h"

MainAlgWorker::MainAlgWorker(SharedRes * sharedRes)
	: mStatisticsTimer(this)
	, mSharedRes(sharedRes)
{}

MainAlgWorker::~MainAlgWorker(){}

void MainAlgWorker::start()
{
	mShutdownFlag = false;
	mEngine = new MatlabEngine(mSharedRes);
	connect(mEngine, SIGNAL(algoStatus(const QString &)), this, SIGNAL(newPauseState(const QString &)));
	connect(mEngine, SIGNAL(isPause(bool)), this, SIGNAL(pause(bool)));
	connect(mEngine, SIGNAL(consoleMessage(const QString &)), this, SLOT(receiveMSGToConsole(const QString &)));
	connect(mEngine, SIGNAL(newData(const QVector<Rule> &)), this, SIGNAL(newData(const QVector<Rule> &)));
	mStatisticsTimer.setInterval(1000);
	connect(&mStatisticsTimer, SIGNAL(timeout()), this, SLOT(formStatistics()));
	mStatisticsTimer.start();
	run();
}

void MainAlgWorker::stop()
{
	mStatisticsTimer.stop();
	mShutdownFlag = true;
}

void MainAlgWorker::setDebugFrequency(int frequency) {
	mFrequency = frequency;
}

void MainAlgWorker::receiveMSGToConsole(const QString & str)
{
	if (mTotalPacketsNum % mFrequency == 0) {
		emit toConsole(str);
	}
}

void MainAlgWorker::setDirectory(const QString & path)
{
	mEngine->setDirectory(path);
}

void MainAlgWorker::formStatistics()
{
	QString tmp;
	QString ToStatus = "Using Matlab: PacketsPerSec = ";
	tmp.setNum(mPacketsPerSecond);
	mPacketsPerSecond = 0;
	ToStatus += tmp;
	ToStatus += " Total packets = ";
	tmp.setNum(mTotalPacketsNum);
	ToStatus += tmp;
	emit sendStatistics(ToStatus);
}

void MainAlgWorker::run()
{
	while (!mShutdownFlag) {
		mEngine->evaluate();
		mPacketsPerSecond++;
		mTotalPacketsNum++;
		QApplication::processEvents();
	}
	delete mEngine;
}

void MainAlgWorker::pauseUnpause()
{
	mEngine->pauseUnpause();
}
