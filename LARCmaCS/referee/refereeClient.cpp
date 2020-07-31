#include "refereeClient.h"

RefereeClient::RefereeClient(SharedRes * sharedRes)
	: mWorker(new RefereeClientWorker())
	, mSharedRes(sharedRes)
{
	qRegisterMetaType<QSharedPointer<RefereeInfo> >("QSharedPointer<RefereeInfo>");

	mWorker->moveToThread(&mThread);

	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(mWorker, SIGNAL(refereeInfoUpdate(const QSharedPointer<RefereeInfo> &)),
			this, SLOT(updateReferee(const QSharedPointer<RefereeInfo> &)));

	mThread.start();
}

RefereeClient::~RefereeClient()
{
	mThread.quit();
	mThread.wait();
}

void RefereeClient::updateReferee(const QSharedPointer<RefereeInfo> &refInfo)
{
	mSharedRes->setRefereeData(refInfo->state, refInfo->commandForTeam, refInfo->isPartOfFieldLeft);
}
