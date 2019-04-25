#include "sceneView.h"
#include <iostream>
#include "sceneViewWorker.h"

using namespace std;

SceneView::SceneView(){};

SceneView::~SceneView()
{
	stop();
	thread.wait(100);
	thread.terminate();
	thread.wait(100);
}

void SceneView::init()
{
	worker.moveToThread(&thread);
	connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
	connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
	connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
}

void SceneView::start()
{
	thread.start();
	emit wstart();
}

void SceneView::stop()
{
	emit wstop();
}
