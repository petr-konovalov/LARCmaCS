#include "mainAlgWorker.h"

#include <QDebug>
#include <QApplication>

#include "message.h"
#include "constants.h"
#include "grSimRobot.h"
#include "defaultRobot.h"

MainAlgWorker::MainAlgWorker()
	: mStatisticsTimer(this)
	, mIsBallInside(false)
{
	mPacketSSL = QSharedPointer<PacketSSL>();
	mIsBallInside = false;
}

MainAlgWorker::~MainAlgWorker(){}

void MainAlgWorker::start()
{
	mShutdownFlag = false;
	init();
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

void MainAlgWorker::setMatlabDebugFrequency(int frequency) {
	mFrequency = frequency;
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


void MainAlgWorker::init(){
	RCConfig rcconfig;
	MlData mtl(rcconfig);
	fmldata = mtl;

	runMatlab();
}

bool MainAlgWorker::getIsSimEnabledFlag()
{
	return mIsSimEnabledFlag;
}

void MainAlgWorker::run()
{
	while (!mShutdownFlag) {
		emit getDataFromReceiver();
		processPacket(mPacketSSL);
		QApplication::processEvents();
	}
	emit finished();
}

void MainAlgWorker::setPacketSSL(const QSharedPointer<PacketSSL> & packetSSL)
{
	mPacketSSL = packetSSL;
}

void MainAlgWorker::updatePauseState()
{
	evalString("ispause=RP.Pause");
	mxArray *mxitpause = engGetVariable(fmldata.ep, "ispause");
	mIsPause = true;
	if (mxitpause != 0) {
		double* itpause = mxGetPr(mxitpause);
		if (itpause != 0) {
			if ((*itpause) == 0) {
				mxArray* mxzMain_End = engGetVariable(fmldata.ep, "zMain_End");
				if (mxzMain_End != 0) {
					double* zMain_End = mxGetPr(mxzMain_End);
					if (zMain_End != 0) {
						if ((*zMain_End) == 0) {
							emit newPauseState("main br");
						} else {
							mIsPause = false;
							emit newPauseState("WORK");
						}
					} else {
						emit newPauseState("-err-z");
					}
				} else {
					emit newPauseState("-err-mz");
				}
			} else {
				emit newPauseState("PAUSE");
			}
		} else {
			emit newPauseState("-err-p"); //Matlab answer corrupted
		}
	} else {
		emit newPauseState("-err-mp"); //Matlab does not respond
	}
}

void MainAlgWorker::processPacket(const QSharedPointer<PacketSSL> & packetssl)
{
	if (packetssl.isNull()) {
		return;
	}
// Заполнение массивов Balls Blues и Yellows и запуск main-функции
	mPacketsPerSecond++;
	mTotalPacketsNum++;

	memcpy(mxGetPr(fmldata.Ball), packetssl->balls, Constants::ballAlgoPacketSize * sizeof(double));
	memcpy(mxGetPr(fmldata.Blue), packetssl->robots_blue, Constants::robotAlgoPacketSize * sizeof(double));
	memcpy(mxGetPr(fmldata.Yellow), packetssl->robots_yellow, Constants::robotAlgoPacketSize * sizeof(double));
	memcpy(mxGetPr(fmldata.ballInside), &mIsBallInside, sizeof(double));


	engPutVariable(fmldata.ep, "Balls", fmldata.Ball);
	engPutVariable(fmldata.ep, "Blues", fmldata.Blue);
	engPutVariable(fmldata.ep, "Yellows", fmldata.Yellow);
	engPutVariable(fmldata.ep, "ballInside", fmldata.ballInside);
	evalString(fmldata.config.file_of_matlab);
// Забираем Rules и очищаем его в воркспейсе

	fmldata.Rule = engGetVariable(fmldata.ep, "Rules");
	double *ruleArray =
			(double *)malloc(Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	if (fmldata.Rule != 0) {
		memcpy(ruleArray, mxGetPr(fmldata.Rule), Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	} else {
		memset(ruleArray, 0, Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	}
	char sendString[256];
	sprintf(sendString, "Rules=zeros(%d, %d);", Constants::ruleAmount, Constants::ruleLength);
	evalString(sendString);

// Разбор пришедшего пакета и переправка его строк на connector

	for (int i = 0; i < Constants::ruleAmount; i++) {
		char newmess[Constants::ruleLength];
		for (int j = 0; j < Constants::ruleLength; j++) {
			newmess[j] = ruleArray[j * Constants::ruleAmount + i];
		}
		if (newmess[0] == 1) {
			QByteArray command;

			int voltage = 12; //fixed while we don't have abilities to change it from algos
			bool simFlag = mIsSimEnabledFlag;
			if (!simFlag) {
				if (!mIsPause) {
					DefaultRobot::formControlPacket(command, newmess[1], newmess[3], newmess[2], newmess[5],
							newmess[6], newmess[4], voltage, 0);
				} else {
					DefaultRobot::formControlPacket(command, newmess[1], 0, 0, 0, 0, 0, voltage, 0);
				}
			} else {
				if (!mIsPause) {
					GrSimRobot::formControlPacket(command, newmess[1], newmess[3], newmess[2], newmess[5],
							newmess[6], newmess[4], voltage, 0);
				} else {
					GrSimRobot::formControlPacket(command, newmess[1], 0, 0, 0, 0, 0, voltage, 0);
				}
			}

			if (newmess[1] == 0) {
				for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
					if (!simFlag) {
						emit sendToConnector(i, command);
					} else {
						QByteArray multiCommand;
						GrSimRobot::formControlPacket(multiCommand, i, newmess[3], newmess[2], newmess[5],
								newmess[6], newmess[4], voltage, 0);
						emit sendToSimConnector(multiCommand);
					}
				}
			}
			if ((newmess[1] > 0) && (newmess[1] <= Constants::maxNumOfRobots)) {
				if (!simFlag) {
					emit sendToConnector(newmess[1], command);
				} else {
					emit sendToSimConnector(command);
				}
			}
		}
	}
	free(ruleArray);
	mxDestroyArray(fmldata.Rule);

	if (mIsPause) { //TODO: add check of remote control
		QByteArray command;
		if (!mIsSimEnabledFlag) {
			for (int i = 1; i <= 12; i++) {
				DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				emit sendToConnector(i, command);
			}
		} else {
			for (int i = 0; i <= 12; i++) {
				GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				emit sendToSimConnector(command); //for more power of remote control
			}
		}
	}

	updatePauseState();
}

void MainAlgWorker::Pause()
{
	evalString("PAUSE();");
}

void MainAlgWorker::runMatlab()
{
	if (!(fmldata.ep = engOpen(NULL))) {
		qDebug() << "Can't open Matlab Engine" << endl;
		fmtlab = false;
		return;
	}

	mMatlabOutputBuffer[Constants::matlabOutputBufferSize - 1] = '\0';
	engOutputBuffer(fmldata.ep, mMatlabOutputBuffer, Constants::matlabOutputBufferSize - 1);
	printf("Matlab Engine is opened\n");

	//-----create Rules-----
	char sendString[256];
	sprintf (sendString, "Rules=zeros(%d, %d)", Constants::ruleAmount, Constants::ruleLength);
	evalString(sendString);

	QString dirPath = "cd " + QCoreApplication::applicationDirPath() + "/MLscripts";
	evalString(dirPath);
	fmtlab = true;
	pause = false;
}

void MainAlgWorker::stop_matlab()
{
	fmtlab = false;
}

void MainAlgWorker::evalString(const QString & s)
{
	engEvalString(fmldata.ep, s.toUtf8().data());
	QString tmp = QString(mMatlabOutputBuffer);
	if (!tmp.contains("\nispause =") && tmp != "" && (mTotalPacketsNum % mFrequency == 0)) {
		emit toMatlabConsole(tmp);
	}
}

void MainAlgWorker::changeBallStatus(bool ballStatus)
{
	mIsBallInside = ballStatus;
}

void MainAlgWorker::changeConnector(bool isSim, const QString &, int)
{
	mIsSimEnabledFlag = isSim;
}
