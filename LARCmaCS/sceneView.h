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

    void init();
    void start();
    void stop();

signals:
    void wstart();
    void wstop();

};
