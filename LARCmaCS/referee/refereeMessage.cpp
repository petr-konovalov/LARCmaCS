#include "refereeMessage.h"

RefereeMessage::RefereeMessage()
	: yellowTeamInfo()
	, blueTeamInfo()
	, packetTimestamp(0)
	, stageTimeLeft(0)
	, commandTimestamp(0)
	, commandCounter(0)
	, ballPlacementPosition(QPointF(0, 0))
	, blueTeamOnPositiveHalf(false)
	, currentActionTimeRemaining(0)
{
	command = Referee::HALT;
	stage = Referee::NORMAL_FIRST_HALF;
	nextCommand = Referee::STOP;
}

RefereeMessage::RefereeMessage(const Referee & packet)
	: yellowTeamInfo(packet.yellow())
	, blueTeamInfo(packet.blue())
{
	packetTimestamp = packet.packet_timestamp();

	stage = packet.stage();
	stageTimeLeft = packet.stage_time_left();

	command = packet.command();
	commandCounter = packet.command_counter();
	commandTimestamp = packet.command_timestamp();

	if (packet.has_designated_position()) {
		ballPlacementPosition = QPointF(packet.designated_position().x(), packet.designated_position().y());
	}
	// this is an optional field
	// what to assign if packet hasn't designated_position?

	blueTeamOnPositiveHalf = packet.blue_team_on_positive_half();

	packet.game_event();

	if (packet.has_next_command()) {
		nextCommand = packet.next_command();
	}
	// this is an optional field
	// what to assign if packet hasn't next_command?

	// parse GameEvents

	// parse ProposedGameEvents

	currentActionTimeRemaining = packet.current_action_time_remaining();
}

QString RefereeMessage::toString(Referee::Stage stage)
{
	switch (stage) {
		case Referee::NORMAL_FIRST_HALF_PRE:
			return "Normal First Half Prep";
		case Referee::NORMAL_FIRST_HALF:
			return "Normal First Half";
		case Referee::NORMAL_HALF_TIME:
			return "Normal Half Time";
		case Referee::NORMAL_SECOND_HALF_PRE:
			return "Normal Second Half Prep";
		case Referee::NORMAL_SECOND_HALF:
			return "Normal Second Half";
		case Referee::EXTRA_TIME_BREAK:
			return "Extra Time Break";
		case Referee::EXTRA_FIRST_HALF_PRE:
			return "Extra First Half Prep";
		case Referee::EXTRA_FIRST_HALF:
			return "Extra First Half";
		case Referee::EXTRA_HALF_TIME:
			return "Extra Half Time";
		case Referee::EXTRA_SECOND_HALF_PRE:
			return "Extra Second Half Prep";
		case Referee::EXTRA_SECOND_HALF:
			return "Extra Second Half";
		case Referee::PENALTY_SHOOTOUT_BREAK:
			return "Penalty Shootout Break";
		case Referee::PENALTY_SHOOTOUT:
			return "Penalty Shootout";
		case Referee::POST_GAME:
			return "Post Game";
	}
}

QString RefereeMessage::toString(Referee::Command command)
{
	switch (command) {
		case Referee::HALT:
			return "Halt";
		case Referee::STOP:
			return "Stop";
		case Referee::NORMAL_START:
			return "Normal Start";
		case Referee::FORCE_START:
			return "Force Start";
		case Referee::PREPARE_KICKOFF_YELLOW:
			return "Yellow Kickoff Prep";
		case Referee::PREPARE_KICKOFF_BLUE:
			return "Blue Kickoff Prep";
		case Referee::PREPARE_PENALTY_YELLOW:
			return "Yellow Penalty Prep";
		case Referee::PREPARE_PENALTY_BLUE:
			return "Blue Penalty Prep";
		case Referee::DIRECT_FREE_YELLOW:
			return "Direct Yellow Free Kick";
		case Referee::DIRECT_FREE_BLUE:
			return "Direct Blue Free Kick";
		case Referee::INDIRECT_FREE_YELLOW:
			return "Indirect Yellow Free Kick";
		case Referee::INDIRECT_FREE_BLUE:
			return "Indirect Blue Free Kick";
		case Referee::TIMEOUT_YELLOW:
			return "Timeout Yellow";
		case Referee::TIMEOUT_BLUE:
			return "Timeout Blue";
		case Referee::GOAL_YELLOW:
			return "Goal Yellow";
		case Referee::GOAL_BLUE:
			return "Goal Blue";
		case Referee::BALL_PLACEMENT_YELLOW:
			return "Ball Placement Yellow";
		case Referee::BALL_PLACEMENT_BLUE:
			return "Ball Placement Blue";
	}
}

Referee::Stage RefereeMessage::getStage() const
{
	return stage;
}

qint32 RefereeMessage::getStageTimeLeft() const
{
	return stageTimeLeft;
}

Referee::Command RefereeMessage::getCommand() const
{
	return command;
}

quint32 RefereeMessage::getCommandCounter() const
{
	return commandCounter;
}

quint64 RefereeMessage::getCommandTimestamp() const
{
	return commandTimestamp;
}

TeamInfo RefereeMessage::getYellowTeamInfo() const
{
	return yellowTeamInfo;
}

TeamInfo RefereeMessage::getBlueTeamInfo() const
{
	return blueTeamInfo;
}

QPointF RefereeMessage::getBallPlacementPosition() const
{
	return ballPlacementPosition;
}

bool RefereeMessage::getBlueTeamOnPositiveHalf() const
{
	return blueTeamOnPositiveHalf;
}

Referee::Command RefereeMessage::getNextCommand() const
{
	return nextCommand;
}

// getter for gameEvents

// getter for ProposedGameEvents

qint32 RefereeMessage::getCurrentActionTimeRemaining() const
{
	return currentActionTimeRemaining;
}
