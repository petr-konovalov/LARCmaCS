#include "settings.h"
#include <QCoreApplication>

Settings::Settings()
	: QSettings(QCoreApplication::applicationFilePath() + ".ini", QSettings::IniFormat){}

QString Settings::configKeyForRobotNum(int n)
{
	return "robot_ip_" + QString::number(n);
}
