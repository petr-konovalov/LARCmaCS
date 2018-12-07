#pragma once

#include <QGraphicsScene>
#include <QGraphicsPathItem>

const int teamUnknown = 0;
const int teamBlue = 1;
const int teamYellow = 2;
const int NA = 0xffff;
const double NAOrientation = 1000.0;

class Robot : public QGraphicsPathItem
{
  public:
    double orientation;     //In degrees
    int teamID;             //Team ID. 0 = blue, 1 = yellow
    int id;                 //ID of the robot in its team
    double x,y;
    double conf;
    int key;
    QString robotLabel;

  private:
    QBrush *brush;
    QPen *pen, *idPen, *confPen;
    QPainterPath robotOutline, robotOutlineCircle, robotID;
    QFont drawFont;

  public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    unsigned long int tStamp;
    void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
//    void update ( qreal x, qreal y, qreal width, qreal height ) { return;}
//    void update ( const QRectF & rect = QRectF() ) {return;}

    Robot();
    Robot ( double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf );
    void SetPose ( double _x, double _y, double _orientation, double _conf );
    ~Robot();
};
