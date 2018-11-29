#pragma once

#include <QVector>
#include <QPoint>

#define TEAM_COUNT 48
#define BALL_COUNT 3
#define TEAM_COUNT_d (TEAM_COUNT * sizeof(double))
#define BALL_COUNT_d (BALL_COUNT * sizeof(double))

struct PacketSSL
{
    PacketSSL();
    PacketSSL(const PacketSSL &ps);
    double robots_blue[48];
    double robots_yellow[48];
    double balls[3];
};
