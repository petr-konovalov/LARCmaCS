#include "sceneViewWorker.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include "timer.h"
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
		Sleep(1);
	}
}
