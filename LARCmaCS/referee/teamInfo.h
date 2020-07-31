#pragma once

#include <QVector>

#include "ssl_referee.pb.h"

struct TeamInfo
{
private:
	// The team's name (empty string if operator has not typed anything).
	QString name;

	// The number of goals scored by the team during normal play and overtime.
	quint32 score;

	// The number of red cards issued to the team since the beginning of the game.
	quint32 redCards;

	// The amount of time (in microseconds) left on each yellow card issued to the team.
	// If no yellow cards are issued, this array has no elements.
	// Otherwise, times are ordered from smallest to largest.
	QVector<quint32> yellowCardTimes;

	// The total number of yellow cards ever issued to the team.
	quint32 yellowCards;

	// The number of timeouts this team can still call.
	// If in a timeout right now, that timeout is excluded.
	quint32 timeouts;

	// The number of microseconds of timeout this team can use.
	quint32 timeoutTime;

	// The pattern number of this team's goalkeeper.
	quint32 goalkeeper;

	// The total number of countable fouls that act towards yellow cards
	quint32 foulCounter;

	// The number of consecutive ball placement failures of this team
	quint32 ballPlacementFailures;

	// Indicate if the team is able and allowed to place the ball
	bool canPlaceBall;

	// The maximum number of bots allowed on the field based on division and cards
	quint32 maxAllowedBots;

public:
	TeamInfo();
	TeamInfo(const Referee::TeamInfo & packet);

	QString getName() const;
	quint32 getScore() const;
	quint32 getRedCards() const;
	QVector<quint32> getYellowCardTimes() const;
	quint32 getYellowCards() const;
	quint32 getTimeouts() const;
	quint32 getTimeoutTime() const;
	quint32 getGoalkeeper() const;
	quint32 getFoulCounter() const;
	quint32 getBallPlacementFailures() const;
	bool getCanPlaceBall() const;
	quint32 getMaxAllowedBots() const;
};
