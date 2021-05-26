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

int Settings::GetAntialiasingSamples() {
  auto antialiasing_type = this->GetValueByKey<AntialiasingType>(
      "main/antialiasing");

  switch (antialiasing_type) {
    case AntialiasingType::kDisableAll:
    case AntialiasingType::kDisableInGame:
      return 0;

    case AntialiasingType::k6x:
      return 6;

    case AntialiasingType::k8x:
      return 8;

    case AntialiasingType::k16x:
      return 16;

    default:
      return 0;
  }
}
