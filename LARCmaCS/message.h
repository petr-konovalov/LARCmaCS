#pragma once
#include <QByteArray>

class Message
{
public:
	Message();

	void setSpeedX(int speedX);
	void setSpeedY(int speedY);
	void setSpeedR(int speedR);
	void setSpeedDribbler(int speedDribbler);
	void setDribblerEnable(int dribblerEnable);
	void setKickVoltageLevel(int kickerVoltageLevel);
	void setKickerChargeEnable(int kickerChargeEnable);
	void setKickUp(int kickUp);
	void setKickForward(int kickForward);
	void setRobotNumber(int number);
	void setBeep(int beep);
	void setAutoKick(int autoKick);
	QByteArray generateByteArray();

private:
	int mSpeedX;
	int mSpeedY;
	int mSpeedR;
	int mSpeedDribbler;
	int mDribblerEnable;
	int mKickerVoltageLevel;
	int mKickerChargeEnable;
	int mKickUp;
	int mKickForward;
public:
	int mRobotNumber;
private:
	int mBeep;
	int mAutoKick;
};
