#include "fieldScene.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTextStream>

FieldScene::FieldScene(QObject *parent) :
	QGraphicsScene(parent)
{
	setBackgroundBrush ( QBrush ( QColor ( 0,0x91,0x19,255 ),Qt::SolidPattern ) );
	//drawMutex = _drawMutex;
	shutdownSoccerView = false;
	//    glWidget = new QGLWidget ( QGLFormat ( QGL::DoubleBuffer ) );
	//    setViewport ( glWidget );
	ksize = 10;
	LoadFieldGeometry();

	ConstructField();
	fieldBrush = new QBrush ( Qt::NoBrush );
	fieldLinePen = new QPen();
	fieldLinePen->setColor ( Qt::white );
	fieldLinePen->setWidth ( 2 );
	fieldLinePen->setJoinStyle ( Qt::MiterJoin );
	fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
	fieldItem->setZValue(0);
	//    drawScale = 0.15;

	//    connect(this, SIGNAL(scaleView(qreal)),rc,SLOT(scaleView(qreal)));
	//    scalingRequested = true;
}

void FieldScene::AddRobot(Robot *robot)
{
	robots.append(robot);
	this->addItem(robot);
}

#ifndef OLD_SSL_PROTO
void FieldScene::UpdateField(SSL_WrapperPacket * packet) {
	LoadFieldGeometry(packet->geometry().field());
	delete packet;
}
#endif

void FieldScene::UpdateRobots(SSL_WrapperPacket * packet)
{
	SSL_DetectionFrame detection = packet->detection();
	int robots_blue_n =  detection.robots_blue_size();
	int robots_yellow_n =  detection.robots_yellow_size();
	//cout << robots_blue_n << " " << robots_yellow_n << endl;
	int i,j,yellowj=0,bluej=0;
	int team=teamBlue;

	SSL_DetectionRobot robot;
	for (i = 0; i < robots_blue_n+robots_yellow_n; i++) {
		//  cout << "i=" << i << endl;
		if (i < robots_blue_n) {
			robot = detection.robots_blue ( i );
			team = teamBlue;
			j=bluej;
		} else {
			robot = detection.robots_yellow ( i-robots_blue_n );
			team = teamYellow;
			j=yellowj;
		}

		double x,y,orientation,conf =robot.confidence();
		int id=NA;
		if (robot.has_robot_id())
			id = robot.robot_id();
		else
			id = NA;
		x = robot.x();
		y = -robot.y();
		if (robot.has_orientation())
			orientation = robot.orientation() *180.0/M_PI;
		else
			orientation = NAOrientation;

		//seek to the next robot of the same camera and team colour
		while ( j<robots.size() && ( robots[j]->key!=detection.camera_id() || robots[j]->teamID!=team ) )
			j++;
		//    cout << "robot size " << robots.size() << endl;

		if ( j+1>robots.size() )
			AddRobot ( new Robot ( x,y,orientation,team,id,detection.camera_id(),conf ) );

		//cout << i << " " << id << " " << x << " " << y << " " << orientation << " " << conf << endl;

		robots[j]->SetPose ( x, -y,orientation,conf );
		QString label;

		if ( id!=NA )
			label.setNum ( id+1,10 );
		else
			label = "?";
		label = label.toUpper();
		if ( label!=robots[j]->robotLabel )
			robots[j]->robotLabel = label;

		j++;

		if ( i<robots_blue_n )
			bluej=j;
		else
			yellowj=j;
	}
	for ( j=bluej;j<robots.size();j++ ) {
		if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamBlue )
			robots[j]->conf=0.0;
	}
	for ( j=yellowj;j<robots.size();j++ ) {
		if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamYellow )
			robots[j]->conf=0.0;
	}

	QVector<QGraphicsEllipseItem*> tmp;
	int cameraID=detection.camera_id();
	while(cameraID+1>ballItems.size())
		ballItems.append(tmp);
	if ( ballItems[cameraID].size() < detection.balls_size() ) {
		//need to allocate some space for the new balls
		QPen pen ( QColor ( 0xcd,0x59,0x00,0xff ) );
		pen.setWidth ( 2 );
		QBrush brush ( QColor ( 0xff,0x81,0x00,0xff ),Qt::SolidPattern );
		while (detection.balls_size()>ballItems[cameraID].size()) {
			ballItems[cameraID].append ( this->addEllipse ( 0,0,12,12,pen,brush ) );
			ballItems[cameraID][ballItems[cameraID].size()-1]->setZValue(2);
		}
	} else if ( ballItems[cameraID].size() >detection.balls_size() ){
	//need to delete some balls
		while(ballItems[cameraID].size()>detection.balls_size()) {
			this->removeItem ( ballItems[cameraID][0] );
			ballItems[cameraID].remove(0);
		}
	}
	for ( int i=0;i<detection.balls_size();i++ ) {
		ballItems[cameraID][i]->setPos ( detection.balls(i).x()/ksize-6,detection.balls(i).y()/ksize-6 );

	}

	delete packet;

	return;
}

void FieldScene::UpdateGeometry(SSL_GeometryFieldSize fieldSize) {
	LoadFieldGeometry(fieldSize);
}

void FieldScene::ClearField()
{
	LoadFieldGeometry();
	this->removeItem(fieldItem);
	field_arcs.clear();
	field_lines.clear();
	ConstructField();
	fieldItem = this->addPath(*field, *fieldLinePen, *fieldBrush);
	for (int i = 0; i < robots.size(); i++) {
		this->removeItem(robots[i]);
	}
	robots.clear();
	for (int i = 0; i < ballItems.size(); i++) {
		for (int j = 0; j < ballItems[i].size(); j++) {
			this->removeItem(ballItems[i][j]);
		}
	}
	ballItems.clear();
}

void FieldScene::ConstructField()
{
	//scene->removeItem(fieldItem);
	field = new QPainterPath();

	QFont qfont;
	qfont.setPixelSize(20);
	field->addText(QPoint(field_length/(2*ksize) + 100 / ksize, field_width/(2*ksize)+200/ksize), qfont , "(" + QString::number(field_length / 2) + ", " + QString::number(-field_width / 2) + ")");
	field->addText(QPoint(-field_length/(2*ksize)-1400/ksize, field_width/(2*ksize)+200/ksize), qfont , "(" + QString::number(-field_length / 2) + ", " + QString::number(-field_width / 2) + ")");
	field->addText(QPoint(-field_length/(2*ksize)-1300/ksize, -field_width/(2*ksize)-100/ksize), qfont , "(" + QString::number(-field_length / 2) + ", " + QString::number(field_width / 2) + ")");
	field->addText(QPoint(field_length/(2*ksize)+100/ksize, -field_width/(2*ksize)-100/ksize), qfont , "(" + QString::number(field_length / 2) + ", " + QString::number(field_width / 2) + ")");

#ifdef OLD_SSL_PROTO
	field->moveTo(0, -field_width / (2 * ksize));
	field->lineTo(0, field_width / (2 * ksize));

	field->addEllipse(-2 * center_circle_radius / (2 * ksize), -center_circle_radius / ksize,
						4 * center_circle_radius / (2 * ksize), 2 * center_circle_radius / ksize);

	field->moveTo(field_length / (2 * ksize), -field_width / (2 * ksize));
	field->lineTo(field_length / (2 * ksize), field_width / (2 * ksize));

	field->moveTo(-field_length / (2 * ksize), -field_width / (2 * ksize));
	field->lineTo(-field_length / (2 * ksize), field_width / (2 * ksize));

	field->moveTo(-field_length / (2 * ksize), -field_width / (2 * ksize));
	field->lineTo(field_length / (2 * ksize), -field_width / (2 * ksize));

	field->moveTo(-field_length / (2 * ksize), field_width / (2 * ksize));
	field->lineTo(field_length / (2 * ksize), field_width / (2 * ksize));

	field->moveTo(field_length / (2 * ksize), goal_width / (2 * ksize));
	field->lineTo((field_length / (2 * ksize) + goal_depth/ksize), goal_width / (2 * ksize));
	field->lineTo((field_length / (2 * ksize) + goal_depth / ksize), -goal_width / (2 * ksize));
	field->lineTo(field_length / (2 * ksize), -goal_width / (2 * ksize));
	field->moveTo((field_length / (2 * ksize) - defense_radius / ksize), defense_stretch / (2 * ksize));
	field->lineTo((field_length / (2 * ksize) - defense_radius / ksize), -defense_stretch / (2 * ksize));
	field->moveTo((field_length / (2 * ksize) - defense_radius / ksize), defense_stretch / (2 * ksize));
	field->arcTo((field_length / (2 * ksize) - defense_radius / ksize), -(defense_radius / ksize - defense_stretch / (2 * ksize)), 2 * defense_radius / ksize, 2 * defense_radius / ksize, 180, 90);
	field->moveTo((field_length / (2 * ksize) - defense_radius / ksize), -defense_stretch / (2 * ksize));
	field->arcTo((field_length / (2 * ksize) - defense_radius / ksize), -(defense_radius / ksize + defense_stretch / (2 * ksize)), 2 * defense_radius / ksize, 2 * defense_radius / ksize, 180, -90);

	field->moveTo( -field_length/(2*ksize),goal_width/(2*ksize) );
	field->lineTo( - ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
	field->lineTo( - ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
	field->lineTo( -field_length/(2*ksize),-goal_width/(2*ksize) );
	field->moveTo( - ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
	field->lineTo( - ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
	field->moveTo( - ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
	field->arcTo( - ( field_length/(2*ksize)+defense_radius/ksize ),- (defense_radius/ksize-defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,0,-90 );
	field->moveTo( - ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
	field->arcTo( - ( field_length/(2*ksize)+defense_radius/ksize ),- ( defense_radius/ksize+defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,0,90 );
#else
	if (field_lines.empty() || field_arcs.empty()) {
		field->moveTo ( 0,-field_width/(2*ksize) );
		field->lineTo ( 0,field_width/(2*ksize) );

		field->addEllipse ( -2*center_circle_radius/(2*ksize),-center_circle_radius/ksize, 4*center_circle_radius/(2*ksize),2*center_circle_radius/ksize );

		field->moveTo ( field_length/(2*ksize),-field_width/(2*ksize) );
		field->lineTo ( field_length/(2*ksize),field_width/(2*ksize) );

		field->moveTo ( -field_length/(2*ksize),-field_width/(2*ksize) );
		field->lineTo ( -field_length/(2*ksize),field_width/(2*ksize) );

		field->moveTo ( -field_length/(2*ksize),-field_width/(2*ksize) );
		field->lineTo ( field_length/(2*ksize),-field_width/(2*ksize) );

		field->moveTo ( -field_length/(2*ksize),field_width/(2*ksize) );
		field->lineTo ( field_length/(2*ksize),field_width/(2*ksize) );

		field->moveTo ( field_length/(2*ksize),goal_width/(2*ksize) );
		field->lineTo ( ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
		field->lineTo ( ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
		field->lineTo ( field_length/(2*ksize),-goal_width/(2*ksize) );
		field->moveTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
		field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
		field->moveTo((field_length/(2*ksize)), penalty_area_width/(2*ksize));
		field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
		field->moveTo((field_length/(2*ksize)), -penalty_area_width/(2*ksize));
		field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );

		field->moveTo ( -field_length/(2*ksize),goal_width/(2*ksize) );
		field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
		field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
		field->lineTo ( -field_length/(2*ksize),-goal_width/(2*ksize) );
		field->moveTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
		field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
		field->moveTo(- (field_length/(2*ksize)), penalty_area_width/(2*ksize));
		field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
		field->moveTo(- (field_length/(2*ksize)), -penalty_area_width/(2*ksize));
		field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
	} else {
		for (int i = 0; i < field_lines.size(); i++) {
			field->moveTo(field_lines[i].p1().x() / ksize, field_lines[i].p1().y() / ksize);
			field->lineTo(field_lines[i].p2().x() / ksize, field_lines[i].p2().y() / ksize);
		}
		for (int i = 0; i < field_arcs.size(); i++) {
			double tmp;
			if (field_arcs[i].a2() < field_arcs[i].a1()) {
				tmp = 2 * M_PI + field_arcs[i].a2() - field_arcs[i].a1();
			} else {
				tmp = field_arcs[i].a2() - field_arcs[i].a1();
			}
			field->moveTo(field_arcs[i].center().x() / ksize + field_arcs[i].radius() / ksize, field_arcs[i].center().y() / ksize);
			field->arcTo(field_arcs[i].center().x() / ksize - field_arcs[i].radius() / ksize, field_arcs[i].center().y() / ksize - field_arcs[i].radius() / ksize, 2 * field_arcs[i].radius() / ksize, 2 * field_arcs[i].radius() / ksize, field_arcs[i].a1(), tmp/0.0175);
		}
	}
#endif
}

void FieldScene::LoadFieldGeometry()
{
#ifdef OLD_SSL_PROTO
	this->line_width = FieldConstantsRoboCup2009::line_width;
	this->field_length = FieldConstantsRoboCup2009::field_length;
	this->field_width = FieldConstantsRoboCup2009::field_width;
	this->boundary_width = FieldConstantsRoboCup2009::boundary_width;
	this->referee_width = FieldConstantsRoboCup2009::referee_width;
	this->goal_width = FieldConstantsRoboCup2009::goal_width;
	this->goal_depth = FieldConstantsRoboCup2009::goal_depth;
	this->goal_wall_width = FieldConstantsRoboCup2009::goal_wall_width;
	this->center_circle_radius = FieldConstantsRoboCup2009::center_circle_radius;
	this->defense_radius = FieldConstantsRoboCup2009::defense_radius;
	this->defense_stretch = FieldConstantsRoboCup2009::defense_stretch;
	this->free_kick_from_defense_dist = FieldConstantsRoboCup2009::free_kick_from_defense_dist;
	this->penalty_spot_from_field_line_dist = FieldConstantsRoboCup2009::penalty_spot_from_field_line_dist;
	this->penalty_line_from_spot_dist = FieldConstantsRoboCup2009::penalty_line_from_spot_dist;
#else
	this->line_width = FieldConstantsRoboCup2018A::kLineThickness;
	this->field_length = FieldConstantsRoboCup2018A::kFieldLength;
	this->field_width = FieldConstantsRoboCup2018A::kFieldWidth;
	this->boundary_width = FieldConstantsRoboCup2018A::kBoundaryWidth;
	this->goal_width = FieldConstantsRoboCup2018A::kGoalWidth;
	this->goal_depth = FieldConstantsRoboCup2018A::kGoalDepth;
	this->center_circle_radius = FieldConstantsRoboCup2018A::kCenterCircleRadius;
	this->penalty_area_depth = FieldConstantsRoboCup2018A::kPenaltyAreaDepth;
	this->penalty_area_width = FieldConstantsRoboCup2018A::kPenaltyAreaWidth;
#endif
}

void FieldScene::LoadFieldGeometry(const SSL_GeometryFieldSize & fieldSize)
{
#ifdef OLD_SSL_PROTO
	this->line_width = fieldSize.line_width();
	this->referee_width = fieldSize.referee_width();
	this->goal_wall_width = fieldSize.goal_wall_width();
	this->center_circle_radius = fieldSize.center_circle_radius();
	this->defense_radius = fieldSize.defense_radius();
	this->defense_stretch = fieldSize.defense_stretch();
	this->free_kick_from_defense_dist = fieldSize.free_kick_from_defense_dist();
	this->penalty_spot_from_field_line_dist = fieldSize.penalty_spot_from_field_line_dist();
	this->penalty_line_from_spot_dist = fieldSize.penalty_line_from_spot_dist();
#else
	this->line_width = FieldConstantsRoboCup2018A::kLineThickness;//fieldSize.line_width();
	field_lines.clear();
	field_arcs.clear();
	for (int i = 0; i < fieldSize.field_lines_size(); i++) {
		field_lines.append(fieldSize.field_lines(i));
	}
	for (int i = 0; i < fieldSize.field_arcs_size(); i++) {
		field_arcs.append(fieldSize.field_arcs(i));
	}

	this->penalty_area_depth = FieldConstantsRoboCup2018A::kPenaltyAreaDepth;
	this->penalty_area_width = FieldConstantsRoboCup2018A::kPenaltyAreaWidth;
	this->center_circle_radius = FieldConstantsRoboCup2018A::kCenterCircleRadius;
#endif

	this->field_length = fieldSize.field_length();
	this->field_width = fieldSize.field_width();
	this->boundary_width = fieldSize.boundary_width();
	this->goal_width = fieldSize.goal_width();
	this->goal_depth = fieldSize.goal_depth();

	this->removeItem ( fieldItem );
	ConstructField();
	fieldItem = this->addPath (*field, *fieldLinePen, *fieldBrush);
}
