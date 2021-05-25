#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

#include <unordered_map>

#include <QSettings>
#include <QStandardPaths>
#include <QString>

class Settings {
 public:
  static Settings& GetInstance();

  template<class T>
  T GetValueByKey(const QString& settings_name);

  template<class T>
  void SetValue(const QString& setting_name, const T& value);

  bool Contains(const QString& setting_name);

 private:
  Settings();

  static Settings* instance_;
  std::unordered_map<QString, QVariant> cache_;
  QSettings settings_;
};

template<class T>
T Settings::GetValueByKey(const QString& settings_name) {
  if (settings_.contains(settings_name)) {
    if (cache_.find(settings_name) == cache_.end()) {
      cache_[settings_name] = settings_.value(settings_name);
    }
    return cache_[settings_name].template value<T>();
  }
  return T();
}

template<class T>
void Settings::SetValue(const QString& setting_name, const T& value) {
  cache_[setting_name] = value;
  settings_.setValue(setting_name, value);
}

#endif  // SETTINGS_SETTINGS_H_
