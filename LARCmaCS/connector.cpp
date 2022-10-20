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
    connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
    connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

    // TODO: Signalling doesn't work for some reason
//    connect(this,
//            SIGNAL(connectorChange(bool, const QString &, int, int, const QString &)),
//            mWorker,
//            SLOT(connectorChanged(bool, const QString &, int, int, const QString &)));

//    connect(this,
//            SLOT(onConnectorChange(bool, const QString &, int, int, const QString &)),
//            mWorker,
//            SLOT(connectorChanged(bool, const QString &, int, int, const QString &)));

    connect(this, SIGNAL(stopped()), mWorker, SLOT(stop()));

    mThread.start();
}

Connector::~Connector()
{
    qDebug() << "CONNECTOR Closing Connector" << endl;
    emit stopped();
    mThread.quit();
    mThread.wait();
}

const QString & Connector::getGrSimIP()
{
    return mWorker->getGrSimIP();
}

unsigned short Connector::getGrSimPort()
{
    return mWorker->getGrSimPort();
}

unsigned short Connector::getRobotPort()
{
    return mWorker->getRobotPort();
}

void Connector::sendNewCommand(const QVector<Rule> & rule)
{
//    qDebug() << "CONNECTOR Sending new command" << endl;
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
//                    qDebug() << "CONNECTOR set last command" << endl;
                    mSharedRes->setLastCommand(command, k);
//                    emit run(k, command);
                } else {
//                    qDebug() << "CONNECTOR set last command sim" << endl;
                    mSharedRes->setLastCommand(command, k);
//                    emit runSim(command, k >= rule.size()/2);
                }
//                oldRule[k] = rule[k];
            //}
        }

    }
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port, int portYellow, const QString & address)
{
//    qDebug() << "CONNECTOR onConnectorChange" << endl;
    mIsSim = isSim;
    // TODO: Signaling doesn't work for some reason
//    emit connectorChange(isSim, ip, port, portYellow, address);
    mWorker->connectorChanged(isSim, ip, port, portYellow, address);
}

void Connector::onPauseChanged(bool status)
{
    qDebug() << "onPauseChanged" << status;
	mIsPause = status;
//    int attemptCount = 10;

	if (mIsPause) {
		QByteArray command;
		if (!mIsSim) {
//            for (int k = 0; k < attemptCount; ++k) {
                for (int i = 0; i < Constants::maxNumOfRobots; i++) {
                    DefaultRobot::formControlPacket(command, i+1, 0, 0, 0, 0, 0, 0, 0);
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
