#include "receiverWorker.h"

ReceiverWorker::ReceiverWorker()
{
    mainalgisfree=true;
    timer_m=clock();
    Time_count=0;
}

void ReceiverWorker::MainAlgFree()
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

void ReceiverWorker::start()
{
    shutdownread = false;
    mainalgisfree = true;
    NewPacket=false;
    MaxPacketFrequencyMod=false;
    cout << "Receiver worker start" << endl;
    run();
}

void ReceiverWorker::stop()
{
    shutdownread = true;
}

void ReceiverWorker::ChangeMaxPacketFrequencyMod(bool state)
{
    MaxPacketFrequencyMod=state;
    cout<<"MaxPacketFrequencyMod = "<<state<<endl;
}
