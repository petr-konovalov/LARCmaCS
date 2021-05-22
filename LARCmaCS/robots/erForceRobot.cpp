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

#include "erForceRobot.h"

ErForceRobot::ErForceRobot(){}


void ErForceRobot::formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
								   bool kickUp, bool kickForward, int kickVoltage,
								   bool enableSpinner, int spinnerSpeed, int autoKick,
								   bool kickerCharge, bool beep)
{
    RobotControl packet;
    bool yellow = false;
	if (numOfRobot >= Constants::maxRobotsInTeam) { //TODO use field TEAM from rule
		yellow = true;
	}
    RobotCommand* robotCommand = packet.add_robot_commands();

    // TODO: What about team? Perhaps deletion is not needed
    robotCommand->set_id(numOfRobot % Constants::maxRobotsInTeam);
    // TODO: Convert (kickVoltage, kickForward, kickUp to speed and angle)
    robotCommand->set_kick_speed(kickVoltage);
    robotCommand->set_kick_angle(0);
    // TODO: How dribbler and spenner are related?
    robotCommand->set_dribbler_speed(0);

    MoveLocalVelocity* localVelocity = robotCommand->mutable_move_command()->mutable_local_velocity();
    localVelocity->set_forward(speedX);
    localVelocity->set_left(speedY);
    localVelocity->set_angular(speedR);

	command.resize(packet.ByteSize());
	packet.SerializeToArray(command.data(), command.size());
}
