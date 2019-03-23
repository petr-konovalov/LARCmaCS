//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    robocup_ssl_client.cpp
  \brief   C++ Implementation: robocup_ssl_client
  \author  Stefan Zickler, 2009
*/
//========================================================================
#include "robocup_ssl_client.h"

RoboCupSSLClient::RoboCupSSLClient(int port,
                     string net_address,
					 string net_interface) : groupAddress(QStringLiteral("224.5.23.2"))
{
  _port=port;
  _net_address=net_address;
  _net_interface=net_interface;
  in_buffer=new char[65536];
  packet = new SSL_WrapperPacket();
}


RoboCupSSLClient::~RoboCupSSLClient()
{
  delete[] in_buffer;
}

void RoboCupSSLClient::close() {
  //mc.close();
  socket.close();
}

bool RoboCupSSLClient::open() {
  close();
  socket.bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress);
  socket.joinMulticastGroup(groupAddress);
  connect(&socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	cout << "$$$$$$$$s\n";
//  if(!mc.open(_port,true,true,blocking)) {
//    fprintf(stderr,"Unable to open UDP network port: %d\n",_port);
//    fflush(stderr);
//    return(false);
//  }


//  Net::Address multiaddr,interface_;
//  multiaddr.setHost(_net_address.c_str(),_port);
//  if(_net_interface.length() > 0){
//    interface_.setHost(_net_interface.c_str(),_port);
//  }else{
//    interface_.setAny();
//  }

//  if(!mc.addMulticast(multiaddr,interface_)) {
//    fprintf(stderr,"Unable to setup UDP multicast\n");
//    fflush(stderr);
//    return(false);
//  }

  return(true);
}

void RoboCupSSLClient::processPendingDatagrams()
{
	QByteArray datagram;
	cout << "@@@\n";
	while (socket.hasPendingDatagrams()) {
		datagram.resize(int(socket.pendingDatagramSize()));
		socket.readDatagram(datagram.data(), datagram.size());
		cout << "@!!!@\n";
		packet->ParseFromArray(datagram.data(), int(socket.pendingDatagramSize()));
		mutex.lock();
		newPacket = true;
		outputPacket = packet;
		mutex.unlock();
		packet = new SSL_WrapperPacket();
	}
}

bool RoboCupSSLClient::receive(SSL_WrapperPacket ** packet) {
	mutex.lock();
	if (newPacket) {
		*packet = outputPacket;
		newPacket = false;
		mutex.unlock();
		return true;
	}
	mutex.unlock();
	return false;
//  Net::Address src;
//  int r=0;
//  r = mc.recv(in_buffer,MaxDataGramSize,src);
//  if (r>0) {
//    fflush(stdout);
//    //decode packet:
//    return packet.ParseFromArray(in_buffer,r);
//  }
//  return false;
}

