#include "robot.h"

#include <iostream>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#define _USE_MATH_DEFINES
#include <math.h>

Robot::Robot()
{
	Robot(0, 0, 0, teamUnknown, 0, 0, 0);
}

int Robot::getCamID()
{
	return mCamID;
}

int Robot::getRobotID()
{
	return mRobotID;
}

int Robot::getTeamID()
{
	return mTeamID;
}

void Robot::setRobotConfidence(double confidence)
{
	mRobotConfidence = confidence;
}

void Robot::setRobotLabel(const QString & label)
{
	mRobotLabel = label;
}

Robot::Robot(double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf)
{
	mRobotConfidence = _conf;
	mCamID = _key;
	setVisible(true);
	setZValue(1);
	mRobotX = _x / 10;
	mRobotY = _y / 10;
	mOrientation = _orientation;
	mTeamID = _teamID;
	mRobotID = _id;

	robotOutline.moveTo(14, 0);
	robotOutline.arcTo(-14, -14, 28, 28, 0, 270);
	robotOutline.closeSubpath();
	robotOutlineCircle.addEllipse(-7, -7, 14, 14);

	if (mRobotID != NA)
		mRobotLabel = QString(QByteArray((char *)&mRobotID, 4).toHex()).mid(1, 1).toUpper();
	else
		mRobotLabel = "?";

	drawFont = QFont("Courier", 8, 2, false);
	robotID.addText(-25, 25, QFont("Courier", 8, 2, false), mRobotLabel);
	switch (mTeamID) {
	case teamBlue:
	{
		brush = QBrush(QColor(0x41, 0x7e, 0xff, 255), Qt::SolidPattern);
		pen = QPen(QColor(0x12, 0x3b, 0xa0, 255));
		break;
	}
	case teamYellow:
	{
		brush = QBrush(QColor(0xff, 0xf3, 0x3e, 255), Qt::SolidPattern);
		pen = QPen(QColor(0xcc, 0x9d, 0x00, 255));
		break;
	}
	default:
	{
		brush = QBrush(QColor(150, 150, 150, 255), Qt::SolidPattern);
		pen = QPen(QColor(70, 70, 70, 255));
		break;
	}
	}
	pen.setWidth(2);
	idPen = QPen(Qt::black);
	idPen.setWidth(0);
	confPen = QPen(Qt::white);
	confPen.setWidth(1);
}

Robot::~Robot()
{
}

QRectF Robot::boundingRect() const
{
	return QRectF (-7.5 + mRobotX, -7.5 - mRobotY, 15, 15);
}

void Robot::paint (QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	if (mRobotConfidence == 0.0)
		return;
	painter->translate(mRobotX, -mRobotY);
	painter->setPen(pen);
	painter->setBrush(brush);
	if (fabs(mOrientation) < 360) {
		painter->rotate(-45 - mOrientation);
		painter->drawPath(robotOutline);
		painter->rotate(45 + mOrientation);
	} else
		painter->drawPath(robotOutlineCircle);

	painter->setPen(idPen);
	painter->setBrush(Qt::black);
	painter->setFont(drawFont);
	//painter->drawPath(robotID);
	painter->drawText(-4.5, -6, 100, 100, 0, mRobotLabel);

	painter->setPen(Qt::NoPen);
	painter->setBrush(brush);
	painter->drawRect(-9, -15, (int)(((double)3) * mRobotConfidence), 3);
	painter->setPen(pen);
	painter->setBrush(QBrush(Qt::white, Qt::NoBrush));
	painter->drawRect(-9, -15, 18, 3);
}

QPainterPath Robot::shape() const
{
	QPainterPath path;
	path.addEllipse(-10, -10, 20, 20);
	return path;
}

void Robot::SetPose (double _x, double _y, double _orientation, double _conf)
{
	mRobotX = _x / 10;
	mRobotY = _y / 10;
	mOrientation = _orientation;
	mRobotConfidence = _conf;
}
