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

SharedRes::SharedRes() : mIPMutex()
{
	qRegisterMetaType<QSharedPointer<SSL_WrapperPacket> >("QSharedPointer<SSL_WrapperPacket>");
	mDetection = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >
			(new QVector<QSharedPointer<SSL_WrapperPacket> >(Constants::numOfCameras));
	for (auto i = 0; i < Constants::numOfCameras; i++) {
		mDetection->replace(i, QSharedPointer<SSL_WrapperPacket>());
	}
	mGeometry = QSharedPointer<SSL_WrapperPacket>();
	mBallInsideData = QSharedPointer<QVector<bool> >(new QVector<bool>(Constants::maxNumOfRobots));
	for (auto i = 0; i < Constants::maxNumOfRobots; i++)
	{
		mBallInsideData->replace(i, false);
	}
	Settings settings;
	for (auto i = 1; i <= Constants::maxNumOfRobots; i++) {
		mIPRobotList[i] = settings.value(Settings::configKeyForRobotNum(i), "").toString();
	}
}

const QSharedPointer<QVector<bool> > & SharedRes::getBallInsideData()
{
	return mBallInsideData;
}

QSharedPointer<SSL_WrapperPacket> SharedRes::getGeometry()
{
	return mGeometry;
}

QSharedPointer<SSL_WrapperPacket> SharedRes::getDetection(int camID)
{
	QReadLocker locker(&mDetectionLock);
	QSharedPointer<SSL_WrapperPacket> res = mDetection->at(camID);
	return res;
}

QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > SharedRes::getDetection()
{
	return mDetection;
}

QString SharedRes::getRobotIP(int id)
{
	QString tmp;
	//mIPMutex.lock();
//	if (id >= 0 && id < mIPRobotList.size()) {
//		tmp = mIPRobotList[id];
//	}
	//mIPMutex.unlock();
	return tmp;
}

void SharedRes::setRobotIP(int id, const QString & ip)
{
	//mIPMutex.lock();
	if (ip.contains(".")) {
		mIPRobotList[id] = ip;
	} else {
		mIPRobotList[id] = "";
	}
	Settings settings;
	settings.setValue(Settings::configKeyForRobotNum(id), ip);
	//mIPMutex.unlock();
}

void SharedRes::setBallInsideData(const QString & ip, bool isBallInside)
{
	//mIPMutex.lock();
//	for (auto i = 0; i < mIPRobotList.size(); i++) {
//		if (mIPRobotList[i] == ip) {
//			mBallInsideData->replace(i, isBallInside);
//			break;
//		}
//	}
	//mIPMutex.unlock();
}

void SharedRes::setDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID)
{
	QWriteLocker locker(&mDetectionLock);
	mDetection->replace(camID, detection);
}

void SharedRes::setGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry)
{
	mGeometry = geometry;
}
