#ifndef CLIENT_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_ABSTRACT_CLIENT_VIEW_H_

#include <memory>

#include <QMainWindow>

#include "Converter/converter.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
  virtual std::shared_ptr<Converter> GetConverter() = 0;
  virtual QPointF GetPlayerToCenterOffset() const = 0;
  virtual void AddKillFeedNotification(QString killer_name,
                                       QString killed_name,
                                       WeaponType weapon_type) = 0;
};

#endif  // CLIENT_ABSTRACT_CLIENT_VIEW_H_
