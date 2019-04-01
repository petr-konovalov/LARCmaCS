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

void ReceiverWorker::newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry)
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
	connect(client, SIGNAL(newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)),
			this, SLOT(newVisionData(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >, QSharedPointer<SSL_WrapperPacket>)), Qt::DirectConnection);
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
