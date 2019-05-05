#include "sceneViewWorker.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include <iostream>

SceneViewWorker::SceneViewWorker(){};

void SceneViewWorker::start(){
	shutdownview = false;
	run();
}

void SceneViewWorker::stop()
{
	shutdownview = true;
}

void SceneViewWorker::run()
{
	while(!shutdownview){
		emit updateView();
		//emit updateRobots();
		QApplication::processEvents();
		QThread::msleep(1);
	}
}
