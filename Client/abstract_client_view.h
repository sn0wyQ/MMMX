#ifndef CLIENT_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_ABSTRACT_CLIENT_VIEW_H_

#include <memory>

#include <QMainWindow>

#include "Converter/converter.h"
#include "Client/KeyController/key_controller.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
  virtual std::shared_ptr<Converter> GetConverter() = 0;
  virtual QPointF GetPlayerToCenterOffset() const = 0;
  virtual void AddKillFeedNotification(const QString& killer_name,
                                       const QString& victim_name,
                                       WeaponType weapon_type) = 0;
  virtual std::shared_ptr<KeyController> GetKeyController() const = 0;
  virtual void AddRespawnNotification(const QString& player_name) = 0;
  virtual void AddPlayerConnectedNotification(const QString& player_name) = 0;
  virtual void AddPlayerDisconnectedNotification(
      const QString& player_name) = 0;
};

#endif  // CLIENT_ABSTRACT_CLIENT_VIEW_H_
