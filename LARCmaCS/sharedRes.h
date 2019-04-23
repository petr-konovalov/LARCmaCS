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
#include "constants.h"

#include <QObject>

class SharedRes : public QObject
{
	Q_OBJECT
private:
	QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > mDetection;
	QSharedPointer<SSL_WrapperPacket> mGeometry;
public:
	SharedRes();
	void setDetection(const QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > & detection);
	void setDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID);
	void setGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry);
	const QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > & getDetection();
	const QSharedPointer<SSL_WrapperPacket> & getGeometry();
};
