#pragma once

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include "packetSSL.h"
#include "fieldScene.h"
#include "sceneViewWorker.h"

struct SceneView: QObject
{
    Q_OBJECT

public:
    SceneViewWorker worker;
    QThread thread;

    explicit SceneView(){}
    ~SceneView() {
        stop();
        thread.wait(100);
        thread.terminate();
        thread.wait(100);
    }

    void init()
    {
        worker.moveToThread(&thread);
        cout << "sceneView init ok" << endl;
        connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
        connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
        connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
    }

    void start()
    {
        thread.start();
        cout << "Scene view thread started" << endl;
        emit wstart();
    }

    void stop() { emit wstop(); }

signals:
    void wstart();
    void wstop();

};
