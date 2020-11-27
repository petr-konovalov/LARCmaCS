#pragma once
#include "constants.h"
#include <cstring>

struct PacketSSL
{
	PacketSSL();
	PacketSSL(const PacketSSL & ps);
	double robots_blue[Constants::robotAlgoPacketSize];
	double robots_yellow[Constants::robotAlgoPacketSize];
    double balls[Constants::ballAlgoPacketSize*Constants::maxBallsInField];
	double fieldInfo[Constants::fieldInfoSize];
};
