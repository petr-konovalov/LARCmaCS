#pragma once

#include <QPointF>

#include "teamInfo.h"
#include "ssl_referee.pb.h"

struct RefereeMessage
{
private:
	// The UNIX timestamp when the packet was sent, in microseconds.
	// Divide by 1,000,000 to get a time_t.
	quint64 packetTimestamp;

	// These are the "coarse" stages of the game.
	Referee::Stage stage;

	// The number of microseconds left in the stage.
	// The following stages have this value; the rest do not:
	// NORMAL_FIRST_HALF
	// NORMAL_HALF_TIME
	// NORMAL_SECOND_HALF
	// EXTRA_TIME_BREAK
	// EXTRA_FIRST_HALF
	// EXTRA_HALF_TIME
	// EXTRA_SECOND_HALF
	// PENALTY_SHOOTOUT_BREAK
	//
	// If the stage runs over its specified time, this value
	// becomes negative.
	qint32 stageTimeLeft;

	// These are the "fine" states of play on the field.
	Referee::Command command;

	// The number of commands issued since startup (mod 2^32).
	quint32 commandCounter;

	// The UNIX timestamp when the command was issued, in microseconds.
	// This value changes only when a new command is issued, not on each packet.
	quint64 commandTimestamp;

	// Information about the two teams.
	TeamInfo yellowTeamInfo;
	TeamInfo blueTeamInfo;

	// The coordinates of the Designated Position. These are measured in
	// millimetres and correspond to SSL-Vision coordinates. These fields are
	// always either both present (in the case of a ball placement command) or
	// both absent (in the case of any other command).
	QPointF ballPlacementPosition;

	// Information about the direction of play.
	// True, if the blue team will have it's goal on the positive x-axis of the ssl-vision coordinate system.
	// Obviously, the yellow team will play on the opposite half.
	bool blueTeamOnPositiveHalf;

	// The command that will be issued after the current stoppage and ball placement to continue the game.
	Referee::Command nextCommand;

	// All game events that were detected since the last RUNNING state.
	// Will be cleared as soon as the game is continued.
	// GameEvent gameEvents;

	// All non-finished proposed game events that may be processed next.
	// Referee::ProposedGameEvent proposedGameEvents;

	// The time in microseconds that is remaining until the current action times out
	// The time will not be reset. It can get negative.
	// An autoRef would raise an appropriate event, if the time gets negative.
	// Possible actions where this time is relevant:
	//  * free kicks
	//  * kickoff, penalty kick, force start
	//  * ball placement
	qint32 currentActionTimeRemaining;

public:
	RefereeMessage();
	RefereeMessage(const Referee & packet);

	Referee::Stage getStage() const;
	qint32 getStageTimeLeft() const;
	Referee::Command getCommand() const;
	quint32 getCommandCounter() const;
	quint64 getCommandTimestamp() const;
	TeamInfo getYellowTeamInfo() const;
	TeamInfo getBlueTeamInfo() const;
	QPointF getBallPlacementPosition() const;
	bool getBlueTeamOnPositiveHalf() const;
	Referee::Command getNextCommand() const;
	// getter for gameEvents
	// getter for ProposedGameEvents
	qint32 getCurrentActionTimeRemaining() const;

	static QString toString(Referee::Stage stage);
	static QString toString(Referee::Command command);
};
