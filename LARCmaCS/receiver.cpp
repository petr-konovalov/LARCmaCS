// Copyright 2019 Dmitrii Iarosh

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "receiver.h"

Receiver::Receiver()
{
	mWorker = new ReceiverWorker();
}

Receiver::~Receiver()
{
	stop();
}

void Receiver::init(SharedRes * sharedRes)
{
	mSharedRes = sharedRes;
	mWorker->moveToThread(&mThread);

	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(this, SIGNAL(wstop()), mWorker, SLOT(stop()));
	connect(mWorker, SIGNAL(finished()), &mThread, SLOT(quit()));
	connect(&mThread, SIGNAL(finished()), &mThread, SLOT(deleteLater()));
	connect(mWorker, SIGNAL(finished()), mWorker, SLOT(deleteLater()));
	connect(mWorker, SIGNAL(clearField()), this, SLOT(clearScene()));
	connect(mWorker, SIGNAL(UpdateSSLFPS(const QString &)), this, SIGNAL(UpdateSSLFPS(const QString &)));
	connect(this, SIGNAL(updateSimulatorMode(bool)), mWorker, SIGNAL(updateSimulatorMode(bool)));
	connect(mWorker, SIGNAL(updateDetection(const QSharedPointer<SSL_WrapperPacket> &, int))
				, this, SLOT(updateDetection(const QSharedPointer<SSL_WrapperPacket> &, int)));
	connect(mWorker, SIGNAL(updateGeometry(const QSharedPointer<SSL_WrapperPacket> &))
				, this, SLOT(updateGeometry(const QSharedPointer<SSL_WrapperPacket> &)));
}

void Receiver::updateDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID)
{
	mSharedRes->setDetection(detection, camID);
}

void Receiver::updateGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry)
{
	mSharedRes->setGeometry(geometry);
}

void Receiver::setDisplayFlag()
{
	mDisplayFlag = true;
}

void Receiver::changeSimulatorMode(bool mode)
{
	emit updateSimulatorMode(mode);
}

void Receiver::clearScene()
{
	emit clearField();
}

void Receiver::start()
{
	mThread.start();
}

void Receiver::stop()
{
	emit wstop();
}
