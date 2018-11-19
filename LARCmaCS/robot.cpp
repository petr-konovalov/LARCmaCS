//#include "fieldScene.h"
#include "robot.h"

#include <iostream>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "timer.h"
#define _USE_MATH_DEFINES
#include <math.h>

Robot::Robot()
{
    Robot(0, 0, 0, teamUnknown, 0, 0, 0);
}

Robot::Robot(double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf)
{
  conf = _conf;
  key = _key;
  setVisible(true);
  setZValue(1);
  x = _x/10;
  y = _y/10;
  orientation = _orientation;
  teamID = _teamID;
  id = _id;

  robotOutline.moveTo(14, 0);
  robotOutline.arcTo(-14, -14, 28, 28, 0, 270);
  robotOutline.closeSubpath();
  robotOutlineCircle.addEllipse(-7, -7, 14, 14);

  if (id != NA)
    robotLabel = QString(QByteArray((char *)&id, 4).toHex()).mid(1, 1).toUpper();
  else
    robotLabel = "?";

  drawFont = QFont("Courier", 8, 2, false);
  robotID.addText(-25, 25, QFont("Courier", 8, 2, false), robotLabel);
  switch (teamID) {
    case teamBlue:
    {
        brush = new QBrush(QColor(0x41, 0x7e, 0xff, 255), Qt::SolidPattern);
        pen = new QPen(QColor(0x12, 0x3b, 0xa0, 255));
        break;
    }
    case teamYellow:
    {
        brush = new QBrush(QColor(0xff, 0xf3, 0x3e, 255), Qt::SolidPattern);
        pen = new QPen(QColor(0xcc, 0x9d, 0x00, 255));
        break;
    }
    default:
    {
        brush = new QBrush(QColor(150, 150, 150, 255), Qt::SolidPattern);
        pen = new QPen(QColor(70, 70, 70, 255));
        break;
    }
  }
  pen->setWidth(2);
  idPen = new QPen(Qt::black);
  idPen->setWidth(0);
  confPen = new QPen(Qt::white);
  confPen->setWidth(1);
  tStamp = GetTimeUSec();
}

Robot::~Robot()
{
  delete brush;
  delete pen;
  delete idPen;
}

QRectF Robot::boundingRect() const
{
  return QRectF ( -7.5+x,-7.5-y,15,15 );
}

void Robot::paint ( QPainter *painter, const QStyleOptionGraphicsItem* , QWidget* )
{
  if ( conf==0.0 )
    return;
  painter->translate ( x,-y );
  painter->setPen ( *pen );
  painter->setBrush ( *brush );
  if ( fabs ( orientation ) <360 )
  {
    painter->rotate ( -45-orientation );
    painter->drawPath ( robotOutline );
    painter->rotate ( 45+orientation );
  }
  else
    painter->drawPath ( robotOutlineCircle );

  painter->setPen ( *idPen );
  painter->setBrush ( Qt::black );
  painter->setFont ( drawFont );
  //painter->drawPath(robotID);
  painter->drawText ( -4.5,-6,100,100,0,robotLabel );

  painter->setPen ( Qt::NoPen );
  painter->setBrush ( *brush );
  painter->drawRect ( -9,-15, ( int ) ( ( ( double ) 3 ) *conf ),3 );
  painter->setPen ( *pen );
  painter->setBrush ( QBrush ( Qt::white, Qt::NoBrush ) );
  painter->drawRect ( -9,-15,18,3 );
}

QPainterPath Robot::shape() const
{
  QPainterPath path;
  path.addEllipse ( -10, -10, 20, 20 );
  return path;
}

void Robot::SetPose ( double _x, double _y, double _orientation, double _conf )
{
  x = _x/10;
  y = _y/10;
  orientation = _orientation;
  conf = _conf;
  tStamp = GetTimeUSec();
}
