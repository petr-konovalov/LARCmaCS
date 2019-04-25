#include "mainAlgWorker.h"

#include <QtWidgets/QApplication>
#include <fstream>
#include "QDebug"
#include "message.h"

int initConfig(RCConfig *config){
	ifstream configFile;
	string line;

	configFile.open("LARCmaCS.cnf", ios::in);

	if(!configFile.is_open()) {
		cerr << "Can't open configuration file" << endl;
		return -1;
	}

	if(!config) {
		cerr << "Can't parse configuration file" << endl;
		return -2;
	}

	while(!configFile.eof()) {
		getline(configFile, line);

		if (line.empty())
			continue;

		if(line.find("rfclient.name") != string::npos) {
			config->name = line.substr(line.find('=') + 1);

			if(config->name.size() > 16) {
				cerr << "[WARNING] rfclient.name value is too long."
					 << endl << "It will be truncated up to 16 characters." << endl;
				config->name.resize(16);
			}
		}

		if(line.find("rfclient.file_of_matlab") != string::npos) {
			string fom = line.substr(line.find('=') + 1);

			if(fom.size() > 16) {
				cerr << "[WARNING] rfclient.file_of_matlab value is too long."
					 << endl << "It will be truncated up to 16 characters." << endl;
				fom.resize(16);
			}
			char *str = new char[fom.length() + 1];
			strcpy(str, fom.c_str());
			config->file_of_matlab = str;
		}

		if(line.find("rfclient.RULE_AMOUNT") != string::npos){
			config->RULE_AMOUNT = atoi(line.substr(line.find('=') + 1).c_str());
		}

		if(line.find("rfclient.RULE_LENGTH") != string::npos){
			config->RULE_LENGTH = atoi(line.substr(line.find('=') + 1).c_str());
		}

		if(line.find("rfclient.BACK_AMOUNT") != string::npos){
			config->BACK_AMOUNT = atoi(line.substr(line.find('=') + 1).c_str());
		}

		if(line.find("rfclient.BACK_LENGTH") != string::npos){
			config->BACK_LENGTH = atoi(line.substr(line.find('=') + 1).c_str());
		}
	}

	configFile.close();
	return 0;
}

MainAlgWorker::MainAlgWorker()
{
	mPacketSSL = QSharedPointer<PacketSSL>();
	QFile addrFile("gamepads.txt");
	if (!addrFile.open(QIODevice::ReadOnly)) {
		qDebug() << "File with addresses is not opened!!!";
	}

	QTextStream in(&addrFile);
	auto allAddrs = in.readAll().split("\n", QString::SkipEmptyParts).filter(QRegExp("^[^#;]"));
	client.initFromList(allAddrs);
	mIsBallInside = false;
}

MainAlgWorker::~MainAlgWorker(){}

void MainAlgWorker::start()
{
	mStatisticsTimer = QSharedPointer<QTimer>(new QTimer());
	mShutdownFlag = false;
	init();
	mStatisticsTimer->setInterval(1000);
	connect(mStatisticsTimer.data(), SIGNAL(timeout()), this, SLOT(formStatistics()));
	mStatisticsTimer->start();
	run();
}

void MainAlgWorker::stop()
{
	mStatisticsTimer->stop();
	mShutdownFlag = true;
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

	cout << "Initialization config file..." << endl;

	if(!initConfig(&rcconfig)) {
		cout << rcconfig.file_of_matlab << endl;
		cout << rcconfig.name << endl;
		cout << rcconfig.BACK_AMOUNT << endl;
		cout << rcconfig.BACK_LENGTH << endl;
		cout << rcconfig.RULE_AMOUNT << endl;
		cout << rcconfig.RULE_LENGTH << endl;

		cout << "...successful" << endl;
	} else {
		cerr << "...bad" << endl;
		rcconfig.name = "Robofootball";
		rcconfig.file_of_matlab = "main";
		rcconfig.RULE_AMOUNT = 5;
		rcconfig.RULE_LENGTH = 7;
		rcconfig.BACK_AMOUNT = 10;
		rcconfig.BACK_LENGTH = 8;
	}

	MlData mtl(rcconfig);
	fmldata = mtl;

	run_matlab();
}

bool MainAlgWorker::getIsSimEnabledFlag()
{
	return mIsSimEnabledFlag;
}

void MainAlgWorker::setEnableSimFlag(bool flag)
{
	mIsSimEnabledFlag = flag;
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
	engEvalString(fmldata.ep, "ispause=RP.Pause");
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

	engEvalString(fmldata.ep, fmldata.config.file_of_matlab);
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
	engEvalString(fmldata.ep, sendString);

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
	engEvalString(fmldata.ep, "PAUSE();");
}

void MainAlgWorker::run_matlab()
{
	if (!(fmldata.ep = engOpen(NULL))) {
		cerr << "Can't open Matlab Engine" << endl;
		fmtlab = false;
		return;
	}

	m_buffer[255] = '\0';
	engOutputBuffer(fmldata.ep, m_buffer, 255);
	printf("Matlab Engine is opened\n");

	//-----create Rules-----
	char sendString[256];
	sprintf (sendString, "Rules=zeros(%d, %d)", Constants::ruleAmount, Constants::ruleLength);
	engEvalString(fmldata.ep, sendString);

	QString dirPath = "cd " + QCoreApplication::applicationDirPath() + "/MLscripts";
	engEvalString(fmldata.ep, dirPath.toUtf8().data());
	fmtlab = true;
	pause = false;
}

void MainAlgWorker::stop_matlab()
{
	fmtlab = false;
}

void MainAlgWorker::EvalString(const QString & s)
{
	engEvalString(fmldata.ep, s.toUtf8().data());
}

void MainAlgWorker::changeBallStatus(bool ballStatus)
{
	mIsBallInside = ballStatus;
}
