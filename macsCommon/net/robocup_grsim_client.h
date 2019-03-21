#pragma once

#include "netraw.h"
#include <string>
#include <QMutex>
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "messages_robocup_ssl_refbox_log.pb.h"
using namespace std;

class RoboCupGrSimClient{
protected:
  static const int MaxDataGramSize = 65536;
  char * in_buffer;
  Net::UDP mc; // multicast client
  QMutex mutex;
  int _port;
  string _net_address;
  string _net_interface;
public:
    RoboCupGrSimClient(int port = 10020,
                     string net_ref_address="224.5.23.2",
                     string net_ref_interface="");

    ~RoboCupGrSimClient();
    bool open(bool blocking=false);
    void close();
    bool receive(SSL_WrapperPacket & packet);

};
