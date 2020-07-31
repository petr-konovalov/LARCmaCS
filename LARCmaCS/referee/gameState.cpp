#include "gameState.h"

GameState::GameState()
	: mState(HALT)
	, mForTeam(NEUTRAL)
	, mOurTeam(NEUTRAL)
	, mPenaltyShootout(false)
	, mPartOfFieldLeft(false)
	, mBallPlacementPosition(QPointF(0, 0))
{}

void GameState::setOurTeam(TeamColour team)
{
	mOurTeam = team;
}

void GameState::updateGameState(const RefereeMessage & message, int prevState)
{
	switch (message.getCommand()) {
		case Referee::HALT:
			mState = HALT;
			mForTeam = NEUTRAL;
			break;
		case Referee::STOP:
			mState = STOP;
			mForTeam = NEUTRAL;
			break;
		case Referee::NORMAL_START:
			if (prevState == PREPARE_KICKOFF || prevState == KICKOFF) {
				mState = KICKOFF;
			}
			if (prevState == PREPARE_PENALTY || prevState == PENALTY) {
				mState = PENALTY;
			}
			break;
		case Referee::FORCE_START:
			mState = RUNNING; // neutral restart
			mForTeam = NEUTRAL;
			break;
		case Referee::PREPARE_KICKOFF_YELLOW:
			mState = PREPARE_KICKOFF;
			mForTeam = YELLOWTEAM;
			break;
		case Referee::PREPARE_KICKOFF_BLUE:
			mState = PREPARE_KICKOFF;
			mForTeam = BLUETEAM;
			break;
		case Referee::PREPARE_PENALTY_YELLOW:
			mState = PREPARE_PENALTY;
			mForTeam = YELLOWTEAM;
			break;
		case Referee::PREPARE_PENALTY_BLUE:
			mState = PREPARE_PENALTY;
			mForTeam = BLUETEAM;
			break;
		case Referee::DIRECT_FREE_YELLOW:
			mState = DIRECT_FREE;
			mForTeam = YELLOWTEAM;
			break;
		case Referee::DIRECT_FREE_BLUE:
			mState = DIRECT_FREE;
			mForTeam = BLUETEAM;
			break;
		case Referee::INDIRECT_FREE_YELLOW:
			mState = INDIRECT_FREE;
			mForTeam = YELLOWTEAM;
			break;
		case Referee::INDIRECT_FREE_BLUE:
			mState = INDIRECT_FREE;
			mForTeam = BLUETEAM;
			break;
		case Referee::TIMEOUT_YELLOW:
			mState = TIMEOUT;
			mForTeam = YELLOWTEAM;
			break;
		case Referee::TIMEOUT_BLUE:
			mState = TIMEOUT;
			mForTeam = BLUETEAM;
			break;
		case Referee::GOAL_YELLOW:
			mState = STOP;
			mForTeam = NEUTRAL;
			break;
		case Referee::GOAL_BLUE:
			mState = STOP;
			mForTeam = NEUTRAL;
			break;
		case Referee::BALL_PLACEMENT_YELLOW:
			mState = BALL_PLACEMENT;
			mForTeam = YELLOWTEAM;
			mBallPlacementPosition = message.getBallPlacementPosition();
			break;
		case Referee::BALL_PLACEMENT_BLUE:
			mState = BALL_PLACEMENT;
			mForTeam = BLUETEAM;
			mBallPlacementPosition = message.getBallPlacementPosition();
			break;
	}

	switch (message.getStage()) {
		case Referee::PENALTY_SHOOTOUT:
			mPenaltyShootout = true;
			break;
		case Referee::POST_GAME:
			mState = POST_GAME;
			mForTeam = NEUTRAL;
			break;
		default:
			mPenaltyShootout = false;
	}

	mPartOfFieldLeft = mOurTeam == BLUETEAM && message.getBlueTeamOnPositiveHalf();
}

void GameState::updateRefereeInfoFromState(RefereeInfo & refInfo)
{
	refInfo.state = mState;
	refInfo.commandForTeam = mForTeam;
	refInfo.isPartOfFieldLeft = mPartOfFieldLeft;
}

const State & GameState::getState() const
{
	return mState;
}

TeamColour GameState::getForTeam() const
{
	return mForTeam;
}

TeamColour GameState::getOurTeam() const
{
	return mOurTeam;
}

const QPointF & GameState::getBallPacementPostion() const
{
	return mBallPlacementPosition;
}

bool GameState::isGameStateForOurTeam() const
{
	return (mForTeam == NEUTRAL) || (mForTeam == mOurTeam);
}

bool GameState::isGameStateForOtherTeam() const
{
	return !isGameStateForOurTeam();
}

bool GameState::isDistanceToBallRequired() const
{
	if (mState == STOP || mState == PREPARE_KICKOFF) {
			return true;
	}

	if (mOurTeam != mForTeam) {
		if (mState == BALL_PLACEMENT || mState == INDIRECT_FREE
				|| mState == DIRECT_FREE || mState == PREPARE_PENALTY) {
			return true;
		}
	}

	return false;
}

bool GameState::isPenaltyShootout() const
{
	return mPenaltyShootout;
}

bool GameState::isPartOfFieldLeft() const
{
	return mPartOfFieldLeft;
}

bool GameState::isRunning() const
{
	return mState == RUNNING;
}
