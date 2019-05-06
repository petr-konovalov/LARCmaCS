#pragma once
#include <QObject>
#include "sharedRes.h"


typedef struct Rule {
	int mSpeedX;
	int mSpeedY;
	int mSpeedR;

	int mSpeedDribbler;
	int mDribblerEnable;

	int mKickerVoltageLevel;
	int mKickerChargeEnable;
	int mKickUp;
	int mKickForward;

	int mBeep;
	int bs_kick_state;
} Rule;


class EngineInterface : public QObject {
    Q_OBJECT

public:
	EngineInterface(SharedRes * sharedRes) : mSharedRes(sharedRes){}
	virtual ~EngineInterface(){}

    /// Вызывает main скрипт конкретного движка
	virtual void evaluate() = 0;

    /// Остановка и продолжение evaluate из GUI (скорее всего из воркера)
	virtual void pauseUnpause() = 0;

    /// Установка пути, где лежит main
	virtual void setDirectory(const QString & path) = 0;

protected:
	SharedRes * mSharedRes;
	bool mIsPause { false };

signals:

    /// Внутренние ошибки движка (для них подразумевается отдельное поле)
	void algoStatus(const QString & message);

    /// Поменялось состояние выполения алгоритмов -- пауза/вычисляются
    void isPause(bool status);

    /// Вывод в консоль всех print из скриптов + внутренние ошибки (stdout+stderr)
    void consoleMessage(const QString & message);

    /// Отправление новых вычисленных управляющих сигналов
	void newData(const QVector<Rule> & rule);
};
