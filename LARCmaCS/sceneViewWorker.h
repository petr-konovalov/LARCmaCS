#pragma once

#include <QObject>
#include "fieldScene.h"

struct SceneViewWorker: public QObject
{
	Q_OBJECT

public:
	explicit SceneViewWorker();

public slots:
	void start();
	void stop();
	//void repaintScene(PacketSSL packetssl);

signals:
	void updateView();
	//    void updateRobots();

private:
	void run();
	bool shutdownview;
};
