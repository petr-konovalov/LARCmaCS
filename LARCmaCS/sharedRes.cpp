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

#include "sharedRes.h"
#include "settings.h"

SharedRes::SharedRes()
{
	qRegisterMetaType<QSharedPointer<SSL_WrapperPacket> >("QSharedPointer<SSL_WrapperPacket>");
	mDetection = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >
			(new QVector<QSharedPointer<SSL_WrapperPacket> >(Constants::numOfCameras));
	for (auto i = 0; i < Constants::numOfCameras; i++) {
		mDetection->replace(i, QSharedPointer<SSL_WrapperPacket>());
	}
	mBarrierState.resize(Constants::maxRobotsInTeam);
}

QVector<bool> SharedRes::getBarrierState()
{
	QReadLocker locker(&mBarrierStateLock);
	return mBarrierState;
}

QSharedPointer<SSL_WrapperPacket> SharedRes::getGeometry()
{
	QReadLocker locker(&mGeometryLock);
	return mGeometry;
}

QSharedPointer<SSL_WrapperPacket> SharedRes::getDetection(int camID)
{
	QReadLocker locker(&mDetectionLock);
	if (camID >= 0 && camID < mDetection->size()) {
		return mDetection->at(camID);
	} else {
		return QSharedPointer<SSL_WrapperPacket>();
	}
}

int SharedRes::getRefereeState()
{
	QReadLocker locker(&mRefereeLock);
	return mRefereeState;
}

int SharedRes::getRefereeTeam()
{
	QReadLocker locker(&mRefereeLock);
	return  mRefereeTeam;
}

bool SharedRes::getRefereePartOfFieldLeft()
{
	QReadLocker locker(&mRefereeLock);
	return mRefereePartOfFieldLeft;
}

QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > SharedRes::getDetection()
{
	return mDetection;
}

int SharedRes::getDetectionSize()
{
	return mDetection->size();
}

void SharedRes::setBarrierState(const QVector<bool> & barrierState)
{
	QWriteLocker locker(&mBarrierStateLock);
	mBarrierState = barrierState;
}

void SharedRes::setDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID)
{
	QWriteLocker locker(&mDetectionLock);
	mDetection->replace(camID, detection);
}

void SharedRes::setGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry)
{
	QWriteLocker locker(&mGeometryLock);
	mGeometry = geometry;
}

void SharedRes::setRefereeData(int state, int team, bool partOfField)
{
	QWriteLocker locker(&mRefereeLock);
	mRefereeState = state;
	mRefereeTeam = team;
	mRefereePartOfFieldLeft = partOfField;
}
