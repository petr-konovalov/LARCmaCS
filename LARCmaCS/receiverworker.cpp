#include "receiverWorker.h"
#include <QtWidgets/QApplication>

ReceiverWorker::ReceiverWorker()
{
	client = new RoboCupVisionClient();
}

ReceiverWorker::~ReceiverWorker()
{
	emit clientClose();
	delete client;
}

int ReceiverWorker::getTotalPacketsNum()
{
	return client->getTotalPacketsNum();
}

int ReceiverWorker::getPacketsPerSecond()
{
	return client->getPacketsPerSecond();
}

RoboCupVisionClient * ReceiverWorker::getClient()
{
	return client;
}

void ReceiverWorker::newVisionData(QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<QVector<bool> > > > detection, QSharedPointer<pair<QSharedPointer<SSL_WrapperPacket>, bool> > geometry)
{
	emit VisionDataReady(detection, geometry);
}

void ReceiverWorker::askForSwapDataVectors()
{
	emit swapDataVectors();
}

void ReceiverWorker::start()
{
	cout << "Receiver worker start" << endl;
	connect(this, SIGNAL(clientOpen(unsigned short)), client, SLOT(open(unsigned short)));
	connect(this, SIGNAL(clientClose()), client, SLOT(close()));
	connect(this, SIGNAL(swapDataVectors()), client, SLOT(swapDataVectors()), Qt::DirectConnection);
    connect(client, SIGNAL(newVisionData(QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<QVector<bool> > > >, QSharedPointer<pair<QSharedPointer<SSL_WrapperPacket>, bool> >)),
            this, SLOT(newVisionData(QSharedPointer<pair<QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<QVector<bool> > > >, QSharedPointer<pair<QSharedPointer<SSL_WrapperPacket>, bool> >)), Qt::DirectConnection);
	emit clientOpen(SSL_VISION_PORT);
}

void ReceiverWorker::ChangeSimulatorMode(bool flag)
{
	if (flag != isSimEnabledFlag) {
		isSimEnabledFlag = flag;
		if (isSimEnabledFlag) {
			emit clientClose();
			emit clientOpen(SIM_VISION_PORT);
		} else {
			emit clientClose();
			emit clientOpen(SSL_VISION_PORT);
		}
	}
}
