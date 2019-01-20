#include "settings.h"
#include <QCoreApplication>

Settings::Settings()
    : QSettings(QCoreApplication::applicationFilePath()+".ini", QSettings::IniFormat)
{
}
