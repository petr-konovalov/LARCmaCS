#pragma once

#include <QObject>

#include "sharedRes.h"


class EngineInterface : public QObject
{
	Q_OBJECT

public:
	EngineInterface(SharedRes * sharedRes) : mSharedRes(sharedRes){}
	virtual ~EngineInterface(){}

	/// Execute main script of engine
	virtual void evaluate() = 0;

	/// Stop and start script evaluation Остановка и продолжение evaluate из GUI (скорее всего из воркера)
	virtual void pauseUnpause() = 0;

	/// Set path to scripts location
	virtual void setDirectory(const QString & path) = 0;

protected:
	SharedRes * mSharedRes;

signals:

	/// Emit when internal engine errors occur
	void algoStatus(const QString & message);

	/// Emit when new execution stage (pause/run)
	void isPause(bool status);

	/// Emit when new output from engine is ready
	void consoleMessage(const QString & message);

	/// Emit when new control signals for robots are ready
	void newData(const QVector<Rule> & rule);
};
