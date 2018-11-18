#include <iostream>
#include "sceneView.h"
#include "sceneViewWorker.h"

using namespace std;

void SceneView::init()
{
    worker.moveToThread(&thread);
    cout << "sceneView init ok" << endl;
    connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
    connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
    connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
}

void SceneView::start()
{
    thread.start();
    cout << "Scene view thread started" << endl;
    emit wstart();
}

void SceneView::stop()
{
    emit wstop();
}
