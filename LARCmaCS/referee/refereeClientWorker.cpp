#include "refereeClientWorker.h"

const QString RefereeClientWorker::hostName = QStringLiteral("224.5.23.1");

RefereeClientWorker::RefereeClientWorker()
	: mSocket(this)
	, mGroupAddress(hostName)
{
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

RefereeClientWorker::~RefereeClientWorker()
{}

void RefereeClientWorker::start()
{
	open(Constants::refereePort);
}

bool RefereeClientWorker::open(qint16 port)
{
	close();
	return mSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)
				&& mSocket.joinMulticastGroup(mGroupAddress);
}

void RefereeClientWorker::close()
{
	mSocket.close();
}

void RefereeClientWorker::processPendingDatagrams()
{
	while (mSocket.hasPendingDatagrams()) {
		QByteArray datagram;
		int datagramSize = static_cast<int>(mSocket.pendingDatagramSize());
		datagram.resize(datagramSize);

		Referee refPacket;
		mSocket.readDatagram(datagram.data(), datagramSize);
		refPacket.ParseFromArray(datagram.data(), datagramSize);

		RefereeMessage message;
		message = RefereeMessage(refPacket);

		GameState gState;
		RefereeInfo tempRefInfo;

		if (mRefInfo != nullptr) {
			gState.updateGameState(message, mRefInfo->state);
		} else {
			gState.updateGameState(message, HALT);
		}

		gState.updateRefereeInfoFromState(tempRefInfo);

		if (mRefInfo != nullptr) {
			if (tempRefInfo.state != mRefInfo->state
					|| tempRefInfo.commandForTeam != mRefInfo->commandForTeam
					|| tempRefInfo.isPartOfFieldLeft != mRefInfo->isPartOfFieldLeft) {
				mRefInfo = QSharedPointer<RefereeInfo>(new RefereeInfo(tempRefInfo));
				emit refereeInfoUpdate(mRefInfo);
			}
		} else {
			mRefInfo = QSharedPointer<RefereeInfo>(new RefereeInfo(tempRefInfo));
			emit refereeInfoUpdate(mRefInfo);
		}
	}
}
