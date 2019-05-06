#pragma once
#include <QObject>
#include "sharedRes.h"


typedef struct Rule {
	int mSpeedX = 0;
	int mSpeedY = 0;
	int mSpeedR = 0;

	int mSpeedDribbler = 0;
	int mDribblerEnable = 0;

	int mKickerVoltageLevel = 12;
	int mKickerChargeEnable = 1;
	int mKickUp = 0;
	int mKickForward = 0;

	int mBeep = 0;
	int bs_kick_state = 0;
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
