// Copyright 2019 Dmitrii Iarosh

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "grSimRobot.h"

GrSimRobot::GrSimRobot(){}

float GrSimRobot::fromPower2Kick(bool isKicked, int voltage)
{
    return isKicked ? voltage : 0; //should be implemented after experiments
}

float GrSimRobot::fromPower2Speed(int power)
{
	return power / 100.0 * 6; //should be implemented after experiments
}

void GrSimRobot::formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
								   bool kickUp, bool kickForward, int kickVoltage,
								   bool enableSpinner, int spinnerSpeed, int autoKick,
								   bool kickerCharge, bool beep)
{
	grSim_Packet packet;
    bool yellow = false;
	if (numOfRobot >= Constants::maxRobotsInTeam) { //TODO use field TEAM from rule
		yellow = true;
	}
	packet.mutable_commands()->set_isteamyellow(yellow);
	packet.mutable_commands()->set_timestamp(0.0);
	grSim_Robot_Command* controls = packet.mutable_commands()->add_robot_commands();

	controls->set_id(numOfRobot % Constants::maxRobotsInTeam);

	//we are not using wheel speed only directional speed!
	controls->set_wheelsspeed(false);
	controls->set_wheel1(0);
	controls->set_wheel2(0);
	controls->set_wheel3(0);
	controls->set_wheel4(0);

	controls->set_veltangent(fromPower2Speed(speedY)); //speed on X axis
	controls->set_velnormal(fromPower2Speed(speedX)); // speed on Y axis
	controls->set_velangular(fromPower2Speed(speedR)); // rotation Speed
	controls->set_kickspeedx(fromPower2Kick(kickForward, kickVoltage));
	controls->set_kickspeedz(fromPower2Kick(kickUp, kickVoltage));
	controls->set_spinner(enableSpinner); //spinner isn't used now

	command.resize(packet.ByteSize());
	packet.SerializeToArray(command.data(), command.size());
}
