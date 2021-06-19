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
#include <QSharedPointer>
#include "ssl_simulation_robot_feedback.pb.h"

class FeedbackReceiverWorker : public QObject
{
	Q_OBJECT

public:
    FeedbackReceiverWorker();
    ~FeedbackReceiverWorker();

public slots:
	void start();
    void changeNetAddress(const QString & ip, int port, int portYellow, const QString & netInterface);

signals:
    void newRobotFeedback(const QSharedPointer<RobotFeedback> &robotFeedback);

private slots:
	void processPendingDatagrams();

private:
    static const QString defaultIp;
    static const QString defaultInterface;
    static const int defaultPort;

    bool open(const QHostAddress ip, unsigned short port, const QString &netInterface);
    void close();

    QNetworkInterface getInterfaceByName(const QString &netInterface);

	QUdpSocket mUdpSocket;
};
