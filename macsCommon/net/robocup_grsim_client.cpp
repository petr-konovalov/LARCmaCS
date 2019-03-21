#include "robocup_grsim_client.h"

RoboCupGrSimClient::RoboCupGrSimClient(int port,
                     string net_address,
                     string net_interface)
{
  _port=port;
  _net_address=net_address;
  _net_interface=net_interface;
  in_buffer=new char[65536];
}


RoboCupGrSimClient::~RoboCupGrSimClient()
{
  delete[] in_buffer;
}

void RoboCupGrSimClient::close() {
  mc.close();
}

bool RoboCupGrSimClient::open(bool blocking) {
  close();
  if(!mc.open(_port,true,true,blocking)) {
    fprintf(stderr,"Unable to open UDP network port: %d\n",_port);
    fflush(stderr);
    return(false);
  }

  Net::Address multiaddr,interface_;
  multiaddr.setHost(_net_address.c_str(),_port);
  if(_net_interface.length() > 0){
    interface_.setHost(_net_interface.c_str(),_port);
  }else{
    interface_.setAny();
  }

  if(!mc.addMulticast(multiaddr,interface_)) {
    fprintf(stderr,"Unable to setup UDP multicast\n");
    fflush(stderr);
    return(false);
  }

  return(true);
}

bool RoboCupGrSimClient::receive(SSL_WrapperPacket & packet) {
  Net::Address src;
  int r=0;
  r = mc.recv(in_buffer,MaxDataGramSize,src);
  if (r>0) {
    fflush(stdout);
    //decode packet:
    bool tmp =  packet.ParseFromArray(in_buffer,r);
    return tmp;
  }
  return false;
}
