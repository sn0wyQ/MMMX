#ifndef CLIENT_GUI_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_GUI_ABSTRACT_CLIENT_VIEW_H_

#include <memory>

#include <QMainWindow>

#include "Converter/converter.h"
#include "Client/KeyController/key_controller.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

namespace ClientWindowTypeWrapper {

Q_NAMESPACE

enum class ClientWindowType {
  // First enum MUST be 0
  kMainMenu = 0,
  kGameView
};

Q_ENUM_NS(ClientWindowType)

}  // namespace ClientWindowTypeWrapper
using ClientWindowType = ClientWindowTypeWrapper::ClientWindowType;
Q_DECLARE_METATYPE(ClientWindowType)

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  ~AbstractClientView() override = default;

  virtual void ConnectToRoom(RoomId room_id) = 0;
  virtual void SetWindow(ClientWindowType window_type) = 0;
  virtual void Update() = 0;
  virtual void UpdateRoomsInfoList() = 0;
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

#endif  // CLIENT_GUI_ABSTRACT_CLIENT_VIEW_H_
