// Copyright 2019 Dmitrii Iarosh

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "matlabEngine.h"
#include <QDebug>
#include <QApplication>

MatlabEngine::MatlabEngine(SharedRes * sharedRes)
	: EngineInterface(sharedRes)
{
	RCConfig rcconfig;
	MlData mtl(rcconfig);
	mMatlabData = mtl;
	runMatlab();
}

MatlabEngine::~MatlabEngine()
{
	engClose(mMatlabData.ep);
}

void MatlabEngine::evaluate()
{
	processPacket(loadVisionData());
}

void MatlabEngine::runMatlab()
{
	if (!(mMatlabData.ep = engOpen(NULL))) {
		qDebug() << "Can't open Matlab Engine" << endl;
		fmtlab = false;
		return;
	}

	mMatlabOutputBuffer[Constants::matlabOutputBufferSize - 1] = '\0';
	engOutputBuffer(mMatlabData.ep, mMatlabOutputBuffer, Constants::matlabOutputBufferSize - 1);
	printf("Matlab Engine is opened\n");

	//-----create Rules-----
	char sendString[256];
	sprintf (sendString, "Rules=zeros(%d, %d)", Constants::ruleAmount, Constants::ruleLength);
	evalString(sendString);

	QString dirPath = "cd " + QCoreApplication::applicationDirPath() + "/MLscripts";
	evalString(dirPath);
	fmtlab = true;
}

void MatlabEngine::evalString(const QString & str)
{
	engEvalString(mMatlabData.ep, str.toUtf8().data());
	QString tmp = QString(mMatlabOutputBuffer);
	if (!tmp.contains("\nispause =") && tmp != "") {
		emit consoleMessage(tmp);
	}
}

QSharedPointer<PacketSSL> MatlabEngine::loadVisionData()
{
	QSharedPointer<PacketSSL> packetSSL(new PacketSSL());

	packetSSL->balls[0] = 0;

	int balls_n, idCam, robots_blue_n, robots_yellow_n;
	for (int i = 0; i < mSharedRes->getDetectionSize(); i++) {
		QSharedPointer<SSL_WrapperPacket> packet = mSharedRes->getDetection(i);
		if (!packet || !packet->IsInitialized()) {
			if (packet)
				qDebug() << "Packet is uninitialized!";
			continue;
		}

		if (packet->has_detection()) {
			SSL_DetectionFrame mDetection = packet->detection();

			idCam = mDetection.camera_id() + 1;
			balls_n = mDetection.balls_size();

			// [Start] Ball info
			if (balls_n != 0) {
				packetSSL->balls[0] = idCam;
				SSL_DetectionBall ball = mDetection.balls(0);
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
						qDebug() << "Error: incorrect blue robot id" << robot.robot_id();
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
						qDebug() << "Error: incorrect yellow robot id" << robot.robot_id();
					}
				}
			}
			// [End] Robot info
		}
	}

	return packetSSL;
}


void MatlabEngine::processPacket(const QSharedPointer<PacketSSL> & packetssl)
{
	if (packetssl.isNull()) {
		return;
	}
// Заполнение массивов Balls Blues и Yellows и запуск main-функции

	memcpy(mxGetPr(mMatlabData.Ball), packetssl->balls, Constants::ballAlgoPacketSize * sizeof(double));
	memcpy(mxGetPr(mMatlabData.Blue), packetssl->robots_blue, Constants::robotAlgoPacketSize * sizeof(double));
	memcpy(mxGetPr(mMatlabData.Yellow), packetssl->robots_yellow, Constants::robotAlgoPacketSize * sizeof(double));
	//memcpy(mxGetPr(mMatlabData.ballInside), &mIsBallInside, sizeof(double));


	engPutVariable(mMatlabData.ep, "Balls", mMatlabData.Ball);
	engPutVariable(mMatlabData.ep, "Blues", mMatlabData.Blue);
	engPutVariable(mMatlabData.ep, "Yellows", mMatlabData.Yellow);
	//engPutVariable(mMatlabData.ep, "ballInside", mMatlabData.ballInside);
	evalString(mMatlabData.config.file_of_matlab);

// Забираем Rules и очищаем его в воркспейсе

	mMatlabData.Rule = engGetVariable(mMatlabData.ep, "Rules");
	double *ruleArray =
			(double *)malloc(Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	if (mMatlabData.Rule != 0) {
		memcpy(ruleArray, mxGetPr(mMatlabData.Rule), Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	} else {
		memset(ruleArray, 0, Constants::ruleAmount * Constants::ruleLength * sizeof(double));
	}
	char sendString[256];
	sprintf(sendString, "Rules=zeros(%d, %d);", Constants::ruleAmount, Constants::ruleLength);
	evalString(sendString);

// Разбор пришедшего пакета и переправка его строк на connector

	QVector<Rule> rule(Constants::ruleAmount);

	for (int i = 0; i < Constants::ruleAmount; i++) {
		if (ruleArray[i] == 1) {
			rule[i].mSpeedX = ruleArray[3 * Constants::ruleAmount + i];
			rule[i].mSpeedY = ruleArray[2 * Constants::ruleAmount + i];
			rule[i].mSpeedR = ruleArray[5 * Constants::ruleAmount + i];
			rule[i].mKickUp = ruleArray[6 * Constants::ruleAmount + i];
			rule[i].mKickForward = ruleArray[4 * Constants::ruleAmount + i];
		}
	}
	emit newData(rule);

	free(ruleArray);
	mxDestroyArray(mMatlabData.Rule);

	updatePauseState();
}

void MatlabEngine::updatePauseState()
{
	evalString("ispause=RP.Pause");
	mxArray *mxitpause = engGetVariable(mMatlabData.ep, "ispause");
	bool pauseStatus = true;
	if (mxitpause != 0) {
		double* itpause = mxGetPr(mxitpause);
		if (itpause != 0) {
			if ((*itpause) == 0) {
				mxArray* mxzMain_End = engGetVariable(mMatlabData.ep, "zMain_End");
				if (mxzMain_End != 0) {
					double* zMain_End = mxGetPr(mxzMain_End);
					if (zMain_End != 0) {
						if ((*zMain_End) == 0) {
							emit algoStatus("main br");
						} else {
							pauseStatus = false;
							emit algoStatus("WORK");
						}
					} else {
						emit algoStatus("-err-z");
					}
				} else {
					emit algoStatus("-err-mz");
				}
			} else {
				emit algoStatus("PAUSE");
			}
		} else {
			emit algoStatus("-err-p"); //Matlab answer corrupted
		}
	} else {
		emit algoStatus("-err-mp"); //Matlab does not respond
	}

	if (mIsPause != pauseStatus) {
		emit isPause(pauseStatus);
		mIsPause = pauseStatus;
	}
}

void MatlabEngine::pauseUnpause()
{
	evalString("PAUSE();");
}

void MatlabEngine::setDirectory(const QString & path)
{
	QString command = "cd " + path;
	qDebug() << "New Matlab directory = " << path;
	evalString(command);
}
