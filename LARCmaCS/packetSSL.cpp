#include "packetSSL.h"

#include <QVector>
#include <QPoint>

PacketSSL::PacketSSL()
{
    memset(robots_blue, 0, TEAM_COUNT_d);
    memset(robots_yellow, 0, TEAM_COUNT_d);
    memset(balls, 0, BALL_COUNT_d);
}

PacketSSL::PacketSSL(const PacketSSL &ps)
{
    memcpy(robots_blue, ps.robots_blue, TEAM_COUNT_d);
    memcpy(robots_yellow, ps.robots_yellow, TEAM_COUNT_d);
    memcpy(balls, ps.balls, BALL_COUNT_d);
}
