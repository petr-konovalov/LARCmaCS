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

#pragma once

#include <QByteArray>
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include "constants.h"

class GrSimRobot
{
public:
	GrSimRobot();
	static void formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
								  bool kickUp, bool kickForward, int kickVoltage,
								  bool enableSpinner, int spinnerSpeed = 0, int autoKick = 0,
								  bool kickerCharge = 1, bool beep = 0);
private:
	static float fromPower2Speed(int power);
	static float fromPower2Kick(bool isKicked, int voltage);
};
