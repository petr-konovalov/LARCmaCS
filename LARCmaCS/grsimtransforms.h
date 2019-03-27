#ifndef GRSIMTRANSFORMS_H
#define GRSIMTRANSFORMS_H

#include <QByteArray>
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#define MAX_NUM_ROBOTS 12 //move to global project define
#define MAX_ROBOTS_IN_TEAM 6 //move to global project define

class GrSimTransforms
{
public:
	GrSimTransforms();
	static void formGrSimControlPacket(QByteArray& command, int numOfRobot, int speedX, int speedY, int speedR, bool kickUp, bool kickForward, int kickVoltage, bool enableSpinner);
private:
	static float fromPower2Speed(int input);
	static float fromPower2Kick(bool input, int voltage);
};

#endif // GRSIMTRANSFORMS_H
