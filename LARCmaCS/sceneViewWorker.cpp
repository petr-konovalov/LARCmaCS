#include "sceneViewWorker.h"
#include <QtWidgets/QApplication>
#include <QThread>

void SceneViewWorker::run(){

    while(!shutdownview){
//        cout << "signal updateView" << endl;
        emit updateView();
        //emit updateRobots();
        QApplication::processEvents();
        Sleep(1);
    }

}
