#pragma once

#include <QUdpSocket>
#include <QMap>
#include <QTimer>
#include <QReadWriteLock>
#include <QThread>

#include "constants.h"
#include "sharedRes.h"
#include "settings.h"
#include "defaultRobot.h"
#include "engineInterface.h"
#include "connectorWorker.h"

using std::map;

class Connector : public QObject
{
	Q_OBJECT

public:
	Connector(SharedRes * sharedRes);
	~Connector();
	const QString & getGrSimIP();
	unsigned short getGrSimPort();
	unsigned short getRobotPort();
    // TODO: Should be private, just for testing
    ConnectorWorker * mWorker;

public slots:
    void onConnectorChange(bool isSim, const QString & ip, int port, int portYellow, const QString &netInterface);
	void sendNewCommand(const QVector<Rule> & rule);
	void onPauseChanged(bool status);

signals:
    void connectorChange(bool isSim, const QString & ip, int port, int portYellow, const QString &netInterface);
    void stopped();

private:
	SharedRes * mSharedRes;
    QThread mThread;

    bool mIsSim { false };
    bool mIsPause { false };
};
