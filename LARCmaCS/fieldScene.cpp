#include "fieldScene.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#define _USE_MATH_DEFINES
#include <math.h>


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

void FieldScene::AddRobot ( Robot *robot )
{
  robots.append ( robot );
  this->addItem ( robot );
}

void FieldScene::UpdateRobots ( SSL_DetectionFrame &detection )
{
  int robots_blue_n =  detection.robots_blue_size();
  int robots_yellow_n =  detection.robots_yellow_size();
  //cout << robots_blue_n << " " << robots_yellow_n << endl;
  int i,j,yellowj=0,bluej=0;
  int team=teamBlue;

  SSL_DetectionRobot robot;
  for ( i = 0; i < robots_blue_n+robots_yellow_n; i++ )
  {
    //  cout << "i=" << i << endl;
    if ( i<robots_blue_n )
    {
      robot = detection.robots_blue ( i );
      team = teamBlue;
      j=bluej;
    }
    else
    {
      robot = detection.robots_yellow ( i-robots_blue_n );
      team = teamYellow;
      j=yellowj;
    }

    double x,y,orientation,conf =robot.confidence();
    int id=NA;
    if ( robot.has_robot_id() )
      id = robot.robot_id();
    else
      id = NA;
    x = robot.x();
    y = robot.y();
    if ( robot.has_orientation() )
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

    robots[j]->SetPose ( x,y,orientation,conf );
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
  for ( j=bluej;j<robots.size();j++ )
  {
    if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamBlue )
      robots[j]->conf=0.0;
  }
  for ( j=yellowj;j<robots.size();j++ )
  {
    if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamYellow )
      robots[j]->conf=0.0;
  }

  QVector<QGraphicsEllipseItem*> tmp;
  int cameraID=detection.camera_id();
  while(cameraID+1>ballItems.size())
    ballItems.append(tmp);
  if ( ballItems[cameraID].size() < detection.balls_size() ){
    //need to allocate some space for the new balls
    QPen pen ( QColor ( 0xcd,0x59,0x00,0xff ) );
    pen.setWidth ( 2 );
    QBrush brush ( QColor ( 0xff,0x81,0x00,0xff ),Qt::SolidPattern );
    while(detection.balls_size()>ballItems[cameraID].size()){
      ballItems[cameraID].append ( this->addEllipse ( 0,0,12,12,pen,brush ) );
      ballItems[cameraID][ballItems[cameraID].size()-1]->setZValue(2);
    }
  }
  else if ( ballItems[cameraID].size() >detection.balls_size() ){
    //need to delete some balls
    while(ballItems[cameraID].size()>detection.balls_size()){
        this->removeItem ( ballItems[cameraID][0] );
        ballItems[cameraID].remove(0);
    }
  }
  for ( int i=0;i<detection.balls_size();i++ )
  {
    ballItems[cameraID][i]->setPos ( detection.balls(i).x()/ksize-6,-detection.balls(i).y()/ksize-6 );
  }
  return;
}

void FieldScene::UpdateGeometry(SSL_GeometryFieldSize fieldSize) {
    LoadFieldGeometry(fieldSize);
}

int FieldScene::UpdateBalls ( QVector<QPointF> &_balls, int cameraID )
{
  QVector<QGraphicsEllipseItem*> tmp;
  while(cameraID+1>ballItems.size())
    ballItems.append(tmp);

  if ( ballItems[cameraID].size() < _balls.size() ){
    //need to allocate some space for the new balls
    QPen pen ( QColor ( 0xcd,0x59,0x00,0xff ) );
    pen.setWidth ( 2 );
    QBrush brush ( QColor ( 0xff,0x81,0x00,0xff ),Qt::SolidPattern );
    while(_balls.size()>ballItems[cameraID].size()){
      ballItems[cameraID].append ( this->addEllipse ( 0,0,12,12,pen,brush ) );
      ballItems[cameraID][ballItems[cameraID].size()-1]->setZValue(2);
    }
  }
  else if ( ballItems[cameraID].size() >_balls.size() ){
    //need to delete some balls
    while(ballItems[cameraID].size()>_balls.size()){
      this->removeItem ( ballItems[cameraID][0] );
      ballItems[cameraID].remove(0);
    }
  }

  for ( int i=0;i<_balls.size();i++ ){
    //Let's update the ball positions now
//      cout << _balls[i].x()/ksize << " " << _balls[i].y()/ksize << endl;
    ballItems[cameraID][i]->setPos ( _balls[i].x()/ksize-6,-_balls[i].y()/ksize-6 );
  }

  int balls = ballItems[cameraID].size();
  return balls;
}

void FieldScene::ConstructField()
{

   //scene->removeItem(fieldItem);
  field = new QPainterPath();

  field->moveTo ( 0,-field_width/(2*ksize) );
  field->lineTo ( 0,field_width/(2*ksize) );

  field->addEllipse ( -2*center_circle_radius/(2*ksize),-center_circle_radius/ksize,
                      4*center_circle_radius/(2*ksize),2*center_circle_radius/ksize );

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
  field->moveTo ( ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
  field->lineTo ( ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
  field->moveTo ( ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
  field->arcTo ( ( field_length/(2*ksize)-defense_radius/ksize ),- ( defense_radius/ksize-defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,180,90 );
  field->moveTo ( ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
  field->arcTo ( ( field_length/(2*ksize)-defense_radius/ksize ),- ( defense_radius/ksize+defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,180,-90 );

  field->moveTo ( -field_length/(2*ksize),goal_width/(2*ksize) );
  field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
  field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
  field->lineTo ( -field_length/(2*ksize),-goal_width/(2*ksize) );
  field->moveTo ( - ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
  field->lineTo ( - ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
  field->moveTo ( - ( field_length/(2*ksize)-defense_radius/ksize ),defense_stretch/(2*ksize) );
  field->arcTo ( - ( field_length/(2*ksize)+defense_radius/ksize ),- (defense_radius/ksize-defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,0,-90 );
  field->moveTo ( - ( field_length/(2*ksize)-defense_radius/ksize ),-defense_stretch/(2*ksize) );
  field->arcTo ( - ( field_length/(2*ksize)+defense_radius/ksize ),- ( defense_radius/ksize+defense_stretch/(2*ksize) ),2*defense_radius/ksize,2*defense_radius/ksize,0,90 );
}

void FieldScene::LoadFieldGeometry()
{
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
}

void FieldScene::LoadFieldGeometry ( SSL_GeometryFieldSize &fieldSize )
{
  this->line_width = fieldSize.line_width();
  // cout << line_width << endln;
  this->field_length = fieldSize.field_length();
  // cout << field_length << endl;
  this->field_width = fieldSize.field_width();
  //cout << field_width << endl;
  this->boundary_width = fieldSize.boundary_width();
  this->referee_width = fieldSize.referee_width();
  this->goal_width = fieldSize.goal_width();
  this->goal_depth = fieldSize.goal_depth();
  this->goal_wall_width = fieldSize.goal_wall_width();
  this->center_circle_radius = fieldSize.center_circle_radius();
  this->defense_radius = fieldSize.defense_radius();
  this->defense_stretch = fieldSize.defense_stretch();
  this->free_kick_from_defense_dist = fieldSize.free_kick_from_defense_dist();
  this->penalty_spot_from_field_line_dist = fieldSize.penalty_spot_from_field_line_dist();
  this->penalty_line_from_spot_dist = fieldSize.penalty_line_from_spot_dist();

  this->removeItem ( fieldItem );
  ConstructField();
  fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
}

//void SoccerScene::setSizeScene(int _size)
//{
////    ksize = _size;
//    qreal sview = (qreal)_size;
//    emit scaleView(sview);
//    this->removeItem( fieldItem );
////    drawMutex->lock();
//    ConstructField();
//    fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
////    drawMutex->unlock();
//}
