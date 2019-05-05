#pragma once

#include <QGraphicsScene>
#include <QGraphicsPathItem>

class Robot : public QGraphicsPathItem
{
public:
	static const int teamUnknown = -1;
	static const int teamBlue = 0;
	static const int teamYellow = 1;
	static const int NA = -1;
	static const int robotNotFound = -1;
	constexpr static const double NAOrientation = 1000.0;

private:
	double mOrientation;     //In degrees
	int mTeamID;             //Team ID. 0 = blue, 1 = yellow
	int mRobotID;                 //ID of the robot in its team
	double mRobotX, mRobotY;
	double mRobotConfidence;
	int mCamID;
	QString mRobotLabel;
	QBrush brush;
	QPen pen, idPen, confPen;
	QPainterPath robotOutline, robotOutlineCircle, robotID;
	QFont drawFont;

public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	Robot();
	Robot(double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf);
	void SetPose(double _x, double _y, double _orientation, double _conf);
	int getTeamID();
	int getCamID();
	int getRobotID();
	void setRobotConfidence(double robotConfidence);
	void setRobotLabel(const QString & label);
	~Robot();
};
