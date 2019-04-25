#include "packetSSL.h"

PacketSSL::PacketSSL()
{
	memset(robots_blue, 0, Constants::robotAlgoPacketSize * sizeof(double));
	memset(robots_yellow, 0, Constants::robotAlgoPacketSize * sizeof(double));
	memset(balls, 0, Constants::ballAlgoPacketSize * sizeof(double));
}

PacketSSL::PacketSSL(const PacketSSL & ps)
{
	memcpy(robots_blue, ps.robots_blue, Constants::robotAlgoPacketSize * sizeof(double));
	memcpy(robots_yellow, ps.robots_yellow, Constants::robotAlgoPacketSize * sizeof(double));
	memcpy(balls, ps.balls, Constants::ballAlgoPacketSize * sizeof(double));
}
