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

#pragma once

#include "messages_robocup_ssl_wrapper.pb.h"

#include <QVector>
#include <QSharedPointer>
#include <QMutex>
#include <QMap>
#include <QReadWriteLock>

#include "constants.h"

class SharedRes : public QObject
{
	Q_OBJECT

public:
	SharedRes();
	int getDetectionSize();
	void setBarrierState(const QVector<bool> & barrierState);
	void setDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID);
	void setGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry);
	void setRefereeData(int state, int team, bool partOfField);
	QVector<bool> getBarrierState();

	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > getDetection();
	QSharedPointer<SSL_WrapperPacket> getGeometry();
	QSharedPointer<SSL_WrapperPacket> getDetection(int camID);
	int getRefereeState();
	int getRefereeTeam();
	bool getRefereePartOfFieldLeft();

private:
	QReadWriteLock mDetectionLock;
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > mDetection;

	QReadWriteLock mGeometryLock;
	QSharedPointer<SSL_WrapperPacket> mGeometry;

	QReadWriteLock mBarrierStateLock;
	QVector<bool> mBarrierState;

	QReadWriteLock mRefereeLock;
	int mRefereeState = 0;
	int mRefereeTeam = 0;
	bool mRefereePartOfFieldLeft = false;
};
