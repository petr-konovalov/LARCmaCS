// Copyright 2019 Anastasiia Kornilova

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

#include <QHostAddress>
#include <QUdpSocket>

class RobotReceiverWorker : public QObject
{
	Q_OBJECT

public:
	RobotReceiverWorker();
	void close();

signals:
	void setBallInsideData(const QString & ip, bool isBallInside);
	void finished();

private slots:
	void start();
	void stop();
	void processPendingDatagrams();

private:
	QUdpSocket udpSocket4;
	QHostAddress groupAddress4;

	QHash<char, char> ballStatuses;
};
