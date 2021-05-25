#include <QStandardPaths>
#include <QSettings>
#include "settings.h"

Settings* Settings::instance_ = nullptr;

Settings::Settings() :
    settings_(QStandardPaths::writableLocation(
        QStandardPaths::StandardLocation::AppConfigLocation),
              QSettings::Format::IniFormat) {}

Settings& Settings::GetInstance() {
  if (!instance_) {
    instance_ = new Settings();
  }
  return *instance_;
}

bool Settings::Contains(const QString& setting_name) {
  return settings_.contains(setting_name);
}
