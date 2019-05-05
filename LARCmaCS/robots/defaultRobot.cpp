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


#include "defaultRobot.h"

DefaultRobot::DefaultRobot(){}

void DefaultRobot::formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
									 bool kickUp, bool kickForward, int kickVoltage,
									 bool enableSpinner, int spinnerSpeed)
{
	Message msg;
	msg.setKickVoltageLevel(kickVoltage);
	msg.setKickerChargeEnable(1);
	msg.setSpeedX(speedX);
	msg.setSpeedY(speedY);
	msg.setSpeedR(speedR);
	msg.setKickForward(kickForward);
	msg.setKickUp(kickUp);
	msg.setDribblerEnable(enableSpinner);
	msg.setSpeedDribbler(spinnerSpeed); //fixed for the first time

	command = msg.generateByteArray();
}