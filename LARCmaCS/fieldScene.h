#pragma once

#include <QGraphicsPathItem>
#include "field_default_constants.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "timer.h"
#include "robot.h"
#include "constants.h"
#include "receiver.h"

using namespace std;

class FieldScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit FieldScene(QObject *parent = 0);
	~FieldScene();
	void setReceiver(Receiver * receiver);
	void start();

public slots:
	void UpdateField(QSharedPointer<QVector<QSharedPointer<SSL_WrapperPacket> > > detection, QSharedPointer<SSL_WrapperPacket> geometry);
	void ClearField();
	void updateFrame();

signals:
	void reDrawScene();

private:
	QTimer mDrawTimer;
	Receiver * mReceiver;
#ifndef OLD_SSL_PROTO
	void UpdateFieldGeometry(QSharedPointer<SSL_WrapperPacket> packet);
#endif
	void UpdateGeometry(SSL_GeometryFieldSize fieldSize);
	void robotsInit();
	void LoadFieldGeometry();
	void LoadFieldGeometry(const SSL_GeometryFieldSize &fieldSize);
	void AddRobot (Robot* robot);
	void UpdateRobots(QSharedPointer<SSL_WrapperPacket> packet);
	//Robots
	QVector<Robot*> robots;
	//balls
	QVector < QVector<QGraphicsEllipseItem*> > ballItems;
	//field
	QPainterPath *field;
	QGraphicsPathItem *fieldItem;
	//brushes and pens
	QBrush *fieldBrush, *ballBrush;
	QPen *fieldPen, *fieldLinePen, *ballPen;
	bool shutdownSoccerView;

	void updateRobot(const SSL_DetectionRobot & robot, int team, unsigned int camID);
	void ConstructField();

	//gain size
	double ksize;

	//Field dimensions and geometry

#ifdef OLD_SSL_PROTO
	double referee_width;
	double goal_wall_width;
	double defense_radius;
	double defense_stretch;
	double free_kick_from_defense_dist;
	double penalty_spot_from_field_line_dist;
	double penalty_line_from_spot_dist;
#else
	QVector<SSL_FieldLineSegment> field_lines;
	QVector<SSL_FieldCicularArc> field_arcs;
	double penalty_area_width;
	double penalty_area_depth;
#endif
	double line_width;
	double field_length;
	double field_width;
	double boundary_width;
	double goal_width;
	double goal_depth;
	double center_circle_radius;
};
