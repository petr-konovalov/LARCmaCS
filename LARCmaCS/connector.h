#pragma once

#include <QUdpSocket>
#include <QMap>
#include <QTimer>
#include <QTextStream>
#include <QFile>

#include "constants.h"
#include "sharedRes.h"
#include "settings.h"
#include "defaultRobot.h"
#include "engineInterface.h"

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
  void onConnectorChange(bool isSim, const QString & ip, int port, int portYellow, const QString &netInterface);
	void sendNewCommand(const QVector<Rule> & rule);
	void onPauseChanged(bool status);
	void run(int N, const QByteArray & command);
    void runSim(const QByteArray & command, bool isYellow);

private:
	QString mGrSimIP;
	unsigned short mGrSimPort;
    unsigned short mGrSimPortYellow;
	SharedRes * mSharedRes;
    QUdpSocket mUdpSocket;
	QTimer mStatisticsTimer;
	static const QString robotBoxIP;
    QFile file;
    QTextStream out;

	bool mIsSim { false };
    static const unsigned short mRobotPort = 10000;
	bool mIsPause { false };

    QVector<Rule> oldRule {*new QVector<Rule>(Constants::ruleAmount)};
};
