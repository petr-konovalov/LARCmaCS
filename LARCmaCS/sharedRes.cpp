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

SharedRes::SharedRes()
{
	qRegisterMetaType<QSharedPointer<SSL_WrapperPacket> >("QSharedPointer<SSL_WrapperPacket>");
	mDetection = QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > >(new QVector<QSharedPointer<SSL_WrapperPacket> >(Constants::numOfCameras));
	for (int i = 0; i < Constants::numOfCameras; i++) {
		mDetection->replace(i, QSharedPointer<SSL_WrapperPacket>());
	}
	mGeometry = QSharedPointer<SSL_WrapperPacket>();
}

const QSharedPointer<SSL_WrapperPacket> & SharedRes::getGeometry()
{
	return mGeometry;
}

const QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > & SharedRes::getDetection()
{
	return mDetection;
}

void SharedRes::setDetection(const QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > & detection)
{
	mDetection = detection;
}

void SharedRes::setDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID)
{
	mDetection->replace(camID, detection);
}

void SharedRes::setGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry)
{
	mGeometry = geometry;
}
