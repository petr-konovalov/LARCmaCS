#pragma once

#include <QGraphicsPathItem>
#include "field_default_constants.h"
#include "robocup_ssl_client.h"
#include "timer.h"
#include "robot.h"

class FieldScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit FieldScene(QObject *parent = 0);

    void AddRobot ( Robot* robot );
    void UpdateRobots ( SSL_DetectionFrame &detection );
    int UpdateBalls ( QVector<QPointF> &_balls, int cameraID );

    void UpdateField(SSL_GeometryFieldSize field);

    void UpdateGeometry(SSL_GeometryFieldSize fieldSize);
    void LoadFieldGeometry();
    void LoadFieldGeometry ( SSL_GeometryFieldSize &fieldSize );

private:

    //Robots
    QVector<Robot*> blueRobots,yellowRobots, robots;
    //balls
    QVector < QVector<QGraphicsEllipseItem*> > ballItems;
    //field
    QPainterPath *field;
    QGraphicsPathItem *fieldItem;
    //brushes and pens
    QBrush *fieldBrush, *ballBrush;
    QPen *fieldPen, *fieldLinePen, *ballPen;
    bool shutdownSoccerView;

    void ConstructField();

    //gain size
    double ksize;

    //Field dimensions and geometry
    vector<SSL_FieldLineSegment> field_lines;
    vector<SSL_FieldCicularArc> field_arcs;
    double line_width;
    double field_length;
    double field_width;
    double boundary_width;
    double goal_width;
    double goal_depth;
    double center_circle_radius;

    double penalty_area_width;
    double penalty_area_depth;
};
