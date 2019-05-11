#include "connector.h"

#include <QMap>

#include "grSimRobot.h"
#include "defaultRobot.h"

const QString Connector::robotBoxIP = QStringLiteral("192.168.0.10");

Connector::Connector(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mUdpSocket(this)
	, mStatisticsTimer(this)
{}

Connector::~Connector()
{}

const QString & Connector::getGrSimIP()
{
	return mGrSimIP;
}

unsigned short Connector::getGrSimPort()
{
	return mGrSimPort;
}

unsigned short Connector::getRobotPort()
{
	return mRobotPort;
}

void Connector::run(int N, const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(robotBoxIP), DefaultRobot::robotPort);
}

void Connector::runSim(const QByteArray & command)
{
	mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), mGrSimPort);
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port)
{
	mIsSim = isSim;

	if (mIsSim) {
		mGrSimIP = ip;
		mGrSimPort = port;
	}
}

void Connector::sendNewCommand(const QVector<Rule> & rule)
{
	for (int k = 0; k < rule.size(); k++) {
		QByteArray command;
		bool simFlag = mIsSim;
		if (!simFlag) {
			if (!mIsPause) {
				DefaultRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
						rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
												rule[k].mDribblerEnable, rule[k].mSpeedDribbler);
			} else {
				DefaultRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
			}
		} else {
			if (!mIsPause) {
				GrSimRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
											  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
											  rule[k].mDribblerEnable, rule[k].mSpeedDribbler);
			} else {
				GrSimRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
			}
		}

		//TODO: sending to all should be implemented in future
//		if (newmess[1] == 0) {
//			for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
//				if (!simFlag) {
//					emit run(i, command);
//				} else {
//					QByteArray multiCommand;
//					GrSimRobot::formControlPacket(multiCommand, i, newmess[3], newmess[2], newmess[5],
//							newmess[6], newmess[4], voltage, 0);
//					emit runSim(multiCommand);
//				}
//			}
//		}

		if (!simFlag) {
			emit run(k, command);
		} else {
			emit runSim(command);
		}
	}
}

void Connector::onPauseChanged(bool status)
{
	qDebug() << "onPauseChanged" << status;
	mIsPause = status;

	if (mIsPause) { //TODO: add check of remote control
		QByteArray command;
		if (!mIsSim) {
			for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
				DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				run(i, command);
			}
		} else {
			for (int i = 0; i <= Constants::maxRobotsInTeam; i++) {
				GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				runSim(command); //for more power of remote control
			}
		}
	}
}
