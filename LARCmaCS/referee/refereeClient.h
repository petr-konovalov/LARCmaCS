#pragma once

#include <QThread>

#include "refereeClientWorker.h"
#include "sharedRes.h"

struct RefereeClient : public QObject
{
	Q_OBJECT

public:
	explicit RefereeClient(SharedRes * sharedRes);
	~RefereeClient();

public slots:
	void updateReferee(const QSharedPointer<RefereeInfo> & refInfo);

private:
	RefereeClientWorker * mWorker;
	QThread mThread;
	SharedRes * mSharedRes;
};
