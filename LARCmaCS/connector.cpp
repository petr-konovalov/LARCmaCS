#include "connector.h"

#include <QMap>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QThread>
#include <QWriteLocker>

#include "grSimRobot.h"
#include "erForceRobot.h"
#include "defaultRobot.h"

Connector::Connector(SharedRes * sharedRes)
    : mSharedRes(sharedRes),
      mWorker(new ConnectorWorker(sharedRes))
{
    mWorker->moveToThread(&mThread);
    mThread.start();
}

Connector::~Connector()
{
    mThread.quit();
    mThread.wait();
}

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

void Connector::sendNewCommand(const QVector<Rule> & rule)
{
    if (!mIsPause) {
        for (int k = 0; k < rule.size(); k++) {
            QByteArray command;
            bool simFlag = mIsSim;
            if (!simFlag) {
                if (!mIsPause) {
                    DefaultRobot::formControlPacket(command, k + 1, rule[k].mSpeedX, rule[k].mSpeedY, -rule[k].mSpeedR,
                            rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
                                                    rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
                                                    rule[k].mKickerChargeEnable, rule[k].mBeep);
                } else {
                    DefaultRobot::formControlPacket(command, k + 1, 0, 0, 0, 0, 0, 0, 0);
                }
            } else {
                if (!mIsPause) {
//                    GrSimRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
//                                                  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
//                                                  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
//                                                  rule[k].mKickerChargeEnable, rule[k].mBeep);
                    ErForceRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
                                                  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
                                                  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
                                                  rule[k].mKickerChargeEnable, rule[k].mBeep);
                } else {
//                    GrSimRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
                    ErForceRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
                }
            }
            /*
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
                rule[k].mBeep != oldRule[k].mBeep) {*/
                if (!simFlag) {
                    mSharedRes->setLastCommand(command, k);
//                    emit run(k, command);
                } else {
                    mSharedRes->setLastCommand(command, k);
//                    emit runSim(command, k >= rule.size()/2);
                }
                oldRule[k] = rule[k];
            //}
        }

    }
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port, int portYellow, const QString &)
{
    mIsSim = isSim;

    if (mIsSim) {
        mGrSimIP = ip;
        mGrSimPort = port;
        mGrSimPortYellow = portYellow;
    }
}

void Connector::onPauseChanged(bool status)
{
	qDebug() << "onPauseChanged" << status;
	mIsPause = status;
    int attemptCount = 10;

	if (mIsPause) {
		QByteArray command;
		if (!mIsSim) {
//            for (int k = 0; k < attemptCount; ++k) {
                for (int i = 0; i < Constants::maxNumOfRobots; i++) {
//                    qDebug() << "Setting command for " << i << endl;
                    DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                    mSharedRes->setLastCommand(command, i);
//                    run(i, command);
                }
                //qDebug() << "Attempt\n";
//                QThread::msleep(100);
//            }
		} else {
            for (int i = 0; i < Constants::maxNumOfRobots; i++) {
//                GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                ErForceRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                mSharedRes->setLastCommand(command, i);
//                runSim(command, i >= Constants::maxRobotsInTeam);
			}
		}
	}
}
