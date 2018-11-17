#pragma once

#include <QObject>
#include "fieldScene.h"

struct SceneViewWorker: public QObject
{
    Q_OBJECT

public:
    explicit SceneViewWorker(){}

public slots:
    void start()
    {
        shutdownview = false;
        cout << "sceneView worker started" << std::endl;
        //cout << "sceneView worker started" << endl;

        run();
    }

    void stop() { shutdownview = true; }
    //void repaintScene(PacketSSL packetssl);

signals:
    void updateView();
//    void updateRobots();

private:
    void run();
    bool shutdownview;
};
