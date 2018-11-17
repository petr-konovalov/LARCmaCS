#include <QObject>
#include "packetSSL.h"
#include "robocup_ssl_client.h"
#include <iostream>
using namespace std;

#include <time.h>

struct ReceiverWorker : public QObject
{
    Q_OBJECT
public:
    explicit ReceiverWorker() {mainalgisfree=true;timer_m=clock(); Time_count=0;}
public slots:
    void MainAlgFree()
    {
       if ((NewPacket) && (MaxPacketFrequencyMod))
       {
           mainalgisfree=false;
           NewPacket=false;
           emit activateMA(packetssl);
           //emit activateGUI();
       }
       else
           mainalgisfree=true;
    }
    void start()
    {
        shutdownread = false;
        mainalgisfree = true;
        NewPacket=false;
        MaxPacketFrequencyMod=false;
        cout << "Receiver worker start" << endl;
        run();
    }

    void stop() { shutdownread = true; }

public slots:
    void ChangeMaxPacketFrequencyMod(bool state)
    {
        MaxPacketFrequencyMod=state;
        cout<<"MaxPacketFrequencyMod = "<<state<<endl;
    }
public:
    SSL_DetectionFrame detection;
    SSL_GeometryFieldSize geometry;
signals:
    void activateGUI();
    void activateMA(PacketSSL packetssl);
    void updatefieldGeometry();
    void UpdateSSLFPS(QString message);

private:
    PacketSSL packetssl;
    clock_t timer_m;
    int Time_count;
private:
    void run();
    RoboCupSSLClient client;
    SSL_WrapperPacket packet;
    bool NewPacket;
    bool shutdownread;
    bool mainalgisfree;
    bool MaxPacketFrequencyMod;
};
