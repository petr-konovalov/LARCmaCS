#include "mainAlgWorker.h"

#include <QDebug>
#include <QApplication>

#include "message.h"
#include "constants.h"

MainAlgWorker::MainAlgWorker(SharedRes * sharedRes)
	: mIsPause(false)
	, mStatisticsTimer(this)
	, mIsBallInside(false)
	, mSharedRes(sharedRes)
{
	mPacketSSL = QSharedPointer<PacketSSL>();
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

QSharedPointer<PacketSSL> MainAlgWorker::loadVisionData()
{
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detectionPackets = mSharedRes->getDetection();

	if (detectionPackets.isNull()) {
		return nullptr;
	}

	QSharedPointer<PacketSSL> packetSSL = QSharedPointer<PacketSSL>(new PacketSSL());

	packetSSL->balls[0] = 0;

	int balls_n, idCam, robots_blue_n, robots_yellow_n;
	SSL_DetectionBall ball;
	for (int i = 0; i < detectionPackets->size(); i++) {
		QSharedPointer<SSL_WrapperPacket> packet = detectionPackets->at(i);
		if (packet.isNull()) {
			continue;
		}

		if (packet->has_detection()) {
			SSL_DetectionFrame mDetection = packet->detection();

			idCam = mDetection.camera_id() + 1;
			balls_n = mDetection.balls_size();

			// [Start] Ball info
			if (balls_n != 0) {
				packetSSL->balls[0] = idCam;
				ball = mDetection.balls(0);
				packetSSL->balls[1] = ball.x();
				packetSSL->balls[2] = ball.y();
			}
			// [End] Ball info

			// [Start] Robot info
			robots_blue_n = mDetection.robots_blue_size();
			robots_yellow_n = mDetection.robots_yellow_size();

			for (int i = 0; i < robots_blue_n; i++) {
				SSL_DetectionRobot robot = mDetection.robots_blue(i);
				if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
					packetSSL->robots_blue[robot.robot_id()] = idCam;
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam] = robot.x();
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 2] = robot.y();
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 3] = robot.orientation();
				} else {
					if (robot.has_robot_id()) {
						qDebug() << robot.robot_id() << " blue" << endl;
					}
				}
			}

			for (int i = 0; i < robots_yellow_n; i++) {
				SSL_DetectionRobot robot = mDetection.robots_yellow(i);
				if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
					packetSSL->robots_yellow[robot.robot_id()] = idCam;
					packetSSL->robots_yellow[robot.robot_id() + 12] = robot.x();
					packetSSL->robots_yellow[robot.robot_id() + 24] = robot.y();
					packetSSL->robots_yellow[robot.robot_id() + 36] = robot.orientation();
				} else {
					if (robot.has_robot_id()) {
						qDebug() << robot.robot_id() << " yellow" << endl;
					}
				}
			}
			// [End] Robot info
		}
	}

	return packetSSL;
}

void MainAlgWorker::run()
{
	while (!mShutdownFlag) {
		QSharedPointer<PacketSSL> packetSSL = loadVisionData();
		if (!packetSSL.isNull())
			processPacket(packetSSL);
		QApplication::processEvents();
	}
}

void MainAlgWorker::updatePauseState()
{
	evalString("ispause=RP.Pause");
	mxArray *mxitpause = engGetVariable(fmldata.ep, "ispause");
	bool pauseStatus = true;
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
							pauseStatus = false;
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

	if (mIsPause != pauseStatus) {
		emit pause(pauseStatus);
		mIsPause = pauseStatus;
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
		QVector<double> newmess;
		newmess.resize(Constants::ruleLength);

		for (int j = 0; j < Constants::ruleLength; j++) {
			newmess[j] = ruleArray[j * Constants::ruleAmount + i];
		}

		emit newData(newmess);
	}
	free(ruleArray);
	mxDestroyArray(fmldata.Rule);

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
