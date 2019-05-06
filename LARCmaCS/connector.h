#pragma once

#include <QUdpSocket>
#include <QMap>
#include <QTimer>

#include "constants.h"
#include "sharedRes.h"
#include "settings.h"
#include "defaultRobot.h"

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

public slots:
	void onConnectorChange(bool isSim, const QString & ip, int port);
	void sendNewCommand(const QVector<double> & newmess);
	void onPauseChanged(bool status);

private:
	void run(int N, const QByteArray & command);
	void runSim(const QByteArray & command);

	QString mGrSimIP;
	unsigned short mGrSimPort;
	SharedRes * mSharedRes;
	QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
	bool mIsSim { false };
	static const unsigned short mRobotPort = 10000;
	bool mIsPause { false };
};
