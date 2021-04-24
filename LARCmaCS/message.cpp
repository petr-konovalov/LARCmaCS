#include "message.h"
#include <QDataStream>

Message::Message() :
	mSpeedX(0)
	, mSpeedY(0)
	, mSpeedR(0)
	, mSpeedDribbler(0)
	, mDribblerEnable(0)
	, mKickerVoltageLevel(0)
	, mKickerChargeEnable(0)
	, mKickUp(0)
	, mKickForward(0)
	, mBeep(0)
	, mAutoKick(0)
{}

void Message::setSpeedX(int speedX)
{
	mSpeedX = speedX;
}

void Message::setSpeedY(int speedY)
{
	mSpeedY = speedY;
}

void Message::setSpeedR(int speedR)
{
	mSpeedR = speedR;
}

void Message::setSpeedDribbler(int speedDribbler)
{
	mSpeedDribbler = speedDribbler;
}

void Message::setDribblerEnable(int dribblerEnable)
{
	mDribblerEnable = dribblerEnable;
}

void Message::setKickVoltageLevel(int kickerVoltageLevel)
{
	mKickerVoltageLevel = kickerVoltageLevel;
}

void Message::setKickerChargeEnable(int kickerChargeEnable)
{
	mKickerChargeEnable = kickerChargeEnable;
}

void Message::setKickUp(int kickUp)
{
	mKickUp = kickUp;
}

void Message::setKickForward(int kickForward)
{
	mKickForward = kickForward;
}

void Message::setRobotNumber(int number)
{
    mRobotNumber = number;
}

void Message::setBeep(int beep)
{
	mBeep = beep;
}
void Message::setAutoKick(int autoKick)
{
	mAutoKick = autoKick;
}

QByteArray Message::generateByteArray()
{
	QByteArray ba;
	QDataStream ds(&ba, QIODevice::WriteOnly);
	ds.setByteOrder(QDataStream::LittleEndian);

	quint8 opcode = 1;
	ds << opcode;
	ds << (quint8)mRobotNumber;

	ds << (qint8)mSpeedY;
	ds << (qint8)mSpeedX;
	ds << (qint8)mSpeedR;

	ds << (quint8)mSpeedDribbler;
	ds << (quint8)mKickerVoltageLevel;
	ds << (quint8)mKickUp;
	ds << (quint8)mKickForward;

	ds << (quint8)mBeep;

	ds << (quint8)mDribblerEnable;
	ds << (quint8)mKickerChargeEnable;
	ds << (quint8)mAutoKick;

	return ba;
}
