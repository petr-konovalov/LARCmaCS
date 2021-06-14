#include "connector.h"

#include <QMap>

#include "grSimRobot.h"
#include "erForceRobot.h"
#include "defaultRobot.h"

const QString Connector::robotBoxIP = QStringLiteral("10.0.120.200");

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
    qInfo() << "Writing to sim" << mGrSimIP << mGrSimPort;
	mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), mGrSimPort);
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port, const QString &)
{
	mIsSim = isSim;

	if (mIsSim) {
		mGrSimIP = ip;
		mGrSimPort = port;
	}
}

void Connector::sendNewCommand(const QVector<Rule> & rule)
{
	if (!mIsPause) {
		for (int k = 0; k < rule.size(); k++) {
			QByteArray command;
			bool simFlag = mIsSim;
			if (!simFlag) {
                if (!mIsPause) {
					DefaultRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
							rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
													rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
													rule[k].mKickerChargeEnable, rule[k].mBeep);
				} else {
					DefaultRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
				}
			} else {
				if (!mIsPause) {
//					GrSimRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
//												  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
//												  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
//												  rule[k].mKickerChargeEnable, rule[k].mBeep);
                    ErForceRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
                                                  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
                                                  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
                                                  rule[k].mKickerChargeEnable, rule[k].mBeep);
                } else {
//					GrSimRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
                    ErForceRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
				}
			}
            if (rule[k].mSpeedX != 0 ||
                rule[k].mSpeedY != 0 ||
                rule[k].mSpeedR != 0 ||
                rule[k].mKickUp != 0 ||
                rule[k].mKickForward != 0 ||
                rule[k].mDribblerEnable != 0 ||
                rule[k].mAutoKick != 0 ||
                rule[k].mSpeedX != oldRule[k].mSpeedX ||
                rule[k].mSpeedY != oldRule[k].mSpeedY ||
                rule[k].mSpeedR != oldRule[k].mSpeedR ||
                rule[k].mKickUp != oldRule[k].mKickUp ||
                rule[k].mKickForward != oldRule[k].mKickForward ||
                rule[k].mKickerVoltageLevel != oldRule[k].mKickerVoltageLevel ||
                rule[k].mDribblerEnable != oldRule[k].mDribblerEnable ||
                rule[k].mSpeedDribbler != oldRule[k].mSpeedDribbler ||
                rule[k].mAutoKick != oldRule[k].mAutoKick ||
                rule[k].mKickerChargeEnable != oldRule[k].mKickerChargeEnable ||
                rule[k].mBeep != oldRule[k].mBeep) {
                if (!simFlag) {
                    emit run(k, command);
                } else {
                    emit runSim(command);
                }
                oldRule[k] = rule[k];
            }
		}

	}
}

void Connector::onPauseChanged(bool status)
{
	qDebug() << "onPauseChanged" << status;
	mIsPause = status;

	if (mIsPause) {
		QByteArray command;
		if (!mIsSim) {
			for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
				DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				run(i, command);
			}
		} else {
			for (int i = 0; i <= Constants::maxRobotsInTeam; i++) {
                //GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                ErForceRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
				runSim(command);
			}
		}
	}
}
