#include "connectorWorker.h"

#include <QMap>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QThread>
#include <QWriteLocker>

#include "grSimRobot.h"
#include "erForceRobot.h"
#include "defaultRobot.h"

const QString ConnectorWorker::robotBoxIP = QStringLiteral("10.0.120.210");

ConnectorWorker::ConnectorWorker(SharedRes * sharedRes)
    : mSharedRes(sharedRes)
    , mUdpSocket(this)
    , mStatisticsTimer(this)
{}

ConnectorWorker::~ConnectorWorker()
{}

const QString & ConnectorWorker::getGrSimIP()
{
    return mGrSimIP;
}

unsigned short ConnectorWorker::getGrSimPort()
{
    return mGrSimPort;
}

unsigned short ConnectorWorker::getRobotPort()
{
    return mRobotPort;
}

void ConnectorWorker::run(int N, const QByteArray & command)
{
//    qDebug() << "CONNECTOR-WORKER write datagram" << endl;
    mUdpSocket.writeDatagram(command, QHostAddress(robotBoxIP), DefaultRobot::robotPort);
}

void ConnectorWorker::runSim(const QByteArray & command, bool isYellow)
{
//    qDebug() << "CONNECTOR-WORKER write SIM datagram" << endl;

//    qDebug() << "before eno" << '\n';
//    const QNetworkInterface iface = QNetworkInterface::interfaceFromName("wlo1");
//    qDebug() << "after eno" << '\n';
//    const QNetworkAddressEntry addrEntry = iface.addressEntries().constFirst();
//    qDebug() << "after addrEntry" << '\n';
//    mUdpSocket.writeDatagram(command, addrEntry.broadcast(), isYellow ? mGrSimPortYellow: mGrSimPort);
//    qDebug() << addrEntry.broadcast() << '\n';
    mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), isYellow ? mGrSimPortYellow: mGrSimPort);
}

void ConnectorWorker::connectorChanged(bool isSim, const QString &ip, int port, int portYellow, const QString & tmp)
{
    qDebug() << "CONNECTOR-WORKER on connector change" << endl;
    mIsSim = isSim;

    if (mIsSim) {
        mGrSimIP = ip;
        mGrSimPort = port;
        mGrSimPortYellow = portYellow;
    }
}


void ConnectorWorker::start()
{
    qDebug() << "CONNECTOR-WORKER start loop" << endl;
    while (!mShutdownFlag) {
//        for (int k = 0; k < Constants::maxNumOfRobots; k++) {
        for (int k = 0; k < 8; k++) {
            QByteArray command = mSharedRes->getLastCommand(k);
//            qDebug() << "CONNECTOR-WORKER sending command " << command << " " << command.size() << endl;
            if (command.size() > 0) {
                if (mIsSim) {
                    runSim(command, k >= Constants::maxRobotsInTeam);
                } else{
                    run(k, command);
                }
            }
        }
        QThread::msleep(10);
    }
}


void ConnectorWorker::stop()
{
    mStatisticsTimer.stop();
    mShutdownFlag = true;
}
