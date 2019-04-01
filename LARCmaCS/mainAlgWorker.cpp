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

	for (int i = 0; i < Constants::maxNumOfRobots; i++)
	{
		Send2BT[i] = true;
	}
	mIsBallInside = false;
}

MainAlgWorker::~MainAlgWorker(){}

void MainAlgWorker::start()
{
	connect(this, SIGNAL(startIteration()), this, SLOT(getPacketFromReceiver()));
	shutdowncomp = false;
	cout << "MainAlg worker start" << endl;
	init();
	emit startIteration();
}

void MainAlgWorker::stop()
{
	shutdowncomp = true;
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

void MainAlgWorker::Send2BTChangeit(bool * send2BT_)
{
	for (int i = 0; i < Constants::maxNumOfRobots; i++) {
		Send2BT[i] = send2BT_[i];
	}
}

bool MainAlgWorker::getIsSimEnabledFlag()
{
	return isSimEnabledFlag;
}

void MainAlgWorker::setEnableSimFlag(bool flag)
{
	isSimEnabledFlag = flag;
}

void MainAlgWorker::getPacketFromReceiver() //run
{
	while (!shutdowncomp) {
		emit getDataFromReceiver();
		processPacket(mPacketSSL);
		QApplication::processEvents();
	}
}

void MainAlgWorker::setPacketSSL(QSharedPointer<PacketSSL> packetSSL)
{
	mPacketSSL = packetSSL;
}

void MainAlgWorker::updatePauseState()
{
	engEvalString(fmldata.ep, "ispause=RP.Pause");
	mxArray *mxitpause = engGetVariable(fmldata.ep, "ispause");
	isPause = true;
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
							isPause = false;
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

void MainAlgWorker::processPacket(QSharedPointer<PacketSSL> packetssl)
{
// Заполнение массивов Balls Blues и Yellows и запуск main-функции

	memcpy(mxGetPr(fmldata.Ball), packetssl->balls, BALL_COUNT_d);
	memcpy(mxGetPr(fmldata.Blue), packetssl->robots_blue, TEAM_COUNT_d);
	memcpy(mxGetPr(fmldata.Yellow), packetssl->robots_yellow, TEAM_COUNT_d);
	memcpy(mxGetPr(fmldata.ballInside), &mIsBallInside, sizeof(double));


	engPutVariable(fmldata.ep, "Balls", fmldata.Ball);
	engPutVariable(fmldata.ep, "Blues", fmldata.Blue);
	engPutVariable(fmldata.ep, "Yellows", fmldata.Yellow);
	engPutVariable(fmldata.ep, "ballInside", fmldata.ballInside);

	engEvalString(fmldata.ep, fmldata.config.file_of_matlab);
// Забираем Rules и очищаем его в воркспейсе

	fmldata.Rule = engGetVariable(fmldata.ep, "Rules");
	double *ruleArray =
			(double *)malloc(fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));
	if (fmldata.Rule != 0)
		memcpy(ruleArray, mxGetPr(fmldata.Rule)
			   , fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));
	else
		memset(ruleArray, 0, fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));

	char sendString[256];
	sprintf(sendString, "Rules=zeros(%d, %d);", fmldata.config.RULE_AMOUNT, fmldata.config.RULE_LENGTH);
	engEvalString(fmldata.ep, sendString);

// Разбор пришедшего пакета и переправка его строк на connector и BT

	for (int i = 0; i < fmldata.config.RULE_AMOUNT; i++) {
		char newmess[100];
		for (int j = 0; j < fmldata.config.RULE_LENGTH; j++) {
			newmess[j] = ruleArray[j * fmldata.config.RULE_AMOUNT + i];
		}
		if (newmess[0] == 1) {
			char * newmessage = new char[100];
			memcpy(newmessage, newmess, 100);
			QByteArray command;

			int voltage = 12; //fixed while we don't have abilities to change it from algos

			bool simFlag = isSimEnabledFlag;
			if (!simFlag) {
				if (!isPause) {
					DefaultRobot::formControlPacket(command, newmess[1], newmess[3], newmess[2], newmess[5], newmess[6], newmess[4], voltage, 0);
				} else {
					DefaultRobot::formControlPacket(command, newmess[1], 0, 0, 0, 0, 0, voltage, 0);
				}
			} else {
				if (!isPause) {
					GrSimRobot::formControlPacket(command, newmess[1], newmess[3], newmess[2], newmess[5], newmess[6], newmess[4], voltage, 0);
				} else {
					GrSimRobot::formControlPacket(command, newmess[1], 0, 0, 0, 0, 0, voltage, 0);
				}
			}

			if (newmess[1] == 0)
				for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
					if (!simFlag) {
						emit sendToConnector(i, command);
					} else {
						emit sendToSimConnector(command);
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

	if (isPause) { //TODO: add check of remote control
		QByteArray command;
		if (!isSimEnabledFlag) {
			for (int i = 1; i <= 12; i++) {
				DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				emit sendToConnector(i, command);
			}
		} else {
			for (int i = 0; i <= 12; i++) {
				GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				//emit sendToSimConnector(command); //for more power of remote control
			}
		}
	}
}

void MainAlgWorker::Pause()
{
	engEvalString(fmldata.ep, "PAUSE();");
}

void MainAlgWorker::run_matlab()
{
	cout << "Work is started..." << endl;

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
	sprintf (sendString, "Rules=zeros(%d, %d)", fmldata.config.RULE_AMOUNT, fmldata.config.RULE_LENGTH);
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

void MainAlgWorker::EvalString(QString s)
{
	engEvalString(fmldata.ep, s.toUtf8().data());
}

void MainAlgWorker::changeBallStatus(bool ballStatus)
{
	mIsBallInside = ballStatus;
}
