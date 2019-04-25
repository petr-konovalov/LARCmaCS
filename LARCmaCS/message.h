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
	QByteArray generateByteArray();
	static quint32 calculateCRC(const QByteArray &buf, int len);

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
};
