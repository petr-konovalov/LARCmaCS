#include "refereeClientWorker.h"

const QString RefereeClientWorker::hostName = QStringLiteral("224.5.23.1");
const QString RefereeClientWorker::defaultInterface = QStringLiteral("eno1");


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
    open(Constants::refereePort, defaultInterface);
}

// TODO: Deduplicate
QNetworkInterface RefereeClientWorker::getInterfaceByName(const QString &netInterface)
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.length(); ++i) {
        if (interfaces.at(i).name() == netInterface) {
            qInfo() << "Found interface " << netInterface;
            return interfaces.at(i);
        };
    }
    qInfo() << "ERROR: No interface found for " << netInterface << ". Using first interface";
    return interfaces.at(0);
}

bool RefereeClientWorker::open(qint16 port, const QString & netInterface)
{
	close();
    QNetworkInterface interface = getInterfaceByName(netInterface);
	return mSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)
                && mSocket.joinMulticastGroup(mGroupAddress, interface);
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

void RefereeClientWorker::changeNetInterface(const QString & netInterface)
{
    qInfo() << "Updating referee interface to" << netInterface;
    open(Constants::refereePort, netInterface);
}
