#include "grsimtransforms.h"

GrSimTransforms::GrSimTransforms(){}

float GrSimTransforms::fromPower2Kick(bool isKicked, int voltage)
{
	if (isKicked) {
		return 2; //should be implemented after experiments
	} else {
		return 0;
	}
}

float GrSimTransforms::fromPower2Speed(int power)
{
	return power / 100.0 * 6; //should be implemented after experiments
}

void GrSimTransforms::formGrSimControlPacket(QByteArray& command, int numOfRobot, int speedX, int speedY, int speedR, bool kickUp, bool kickForward, int kickVoltage, bool enableSpinner)
{
	grSim_Packet packet;
	bool yellow = false;
	if (numOfRobot >= MAX_ROBOTS_IN_TEAM) {
		yellow = true;
	}
	packet.mutable_commands()->set_isteamyellow(yellow);
	packet.mutable_commands()->set_timestamp(0.0);
	grSim_Robot_Command* controls = packet.mutable_commands()->add_robot_commands();

	controls->set_id(numOfRobot % MAX_ROBOTS_IN_TEAM);

	//we are not using wheel speed only directional speed!
	controls->set_wheelsspeed(false);
	controls->set_wheel1(0);
	controls->set_wheel2(0);
	controls->set_wheel3(0);
	controls->set_wheel4(0);

	controls->set_veltangent(fromPower2Speed(speedX)); //speed on X axis
	controls->set_velnormal(-fromPower2Speed(speedY)); // speed on Y axis
	controls->set_velangular(fromPower2Speed(speedR)); // rotation Speed

	controls->set_kickspeedx(fromPower2Kick(kickForward, kickVoltage));
	controls->set_kickspeedz(fromPower2Kick(kickUp, kickVoltage));
	controls->set_spinner(enableSpinner); //spinner isn't used now

	command.resize(packet.ByteSize());
	packet.SerializeToArray(command.data(), command.size());
}
