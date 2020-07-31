#pragma once

#include <QUdpSocket>

#include "constants.h"
#include "gameState.h"
#include "refereeInfo.h"
#include "refereeUtils.h"
#include "refereeMessage.h"

struct RefereeClientWorker : public QObject
{
	Q_OBJECT

public:
	RefereeClientWorker();
	~RefereeClientWorker();

public slots:
	void start();

private slots:
	void processPendingDatagrams();

signals:
	void refereeInfoUpdate(const QSharedPointer<RefereeInfo> & refInfo);

private:
	bool open(qint16 port);
	void close();

	static const QString hostName;
	QUdpSocket mSocket;
	QHostAddress mGroupAddress;
	QSharedPointer<RefereeInfo> mRefInfo;
};
