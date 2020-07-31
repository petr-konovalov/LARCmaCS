#pragma once

#include <QObject>

#include "refereeInfo.h"
#include "refereeUtils.h"
#include "refereeMessage.h"

struct GameState : public QObject
{
	Q_OBJECT

public:
	GameState();
	void setOurTeam(TeamColour ourTeam);
	void updateGameState(const RefereeMessage & message, int prevState);
	void updateRefereeInfoFromState(RefereeInfo & refInfo);

	const State & getState() const;
	TeamColour getForTeam() const;
	TeamColour getOurTeam() const;
	const QPointF & getBallPacementPostion() const;

	bool isGameStateForOurTeam() const;
	bool isGameStateForOtherTeam() const;

	bool isDistanceToBallRequired() const;
	bool isPenaltyShootout() const;
	bool isPartOfFieldLeft() const;
	bool isRunning() const;

private:
	State mState;
	TeamColour mForTeam;
	TeamColour mOurTeam;
	bool mPartOfFieldLeft;
	bool mPenaltyShootout;
	QPointF mBallPlacementPosition;
};
