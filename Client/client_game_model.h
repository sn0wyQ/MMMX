#ifndef CLIENT_CLIENT_GAME_MODEL_H_
#define CLIENT_CLIENT_GAME_MODEL_H_

#include <deque>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QList>
#include <QVariant>

#include "Model/game_model.h"
#include "Room/room_info.h"

class ClientGameModel : public GameModel {
 public:
  ClientGameModel() = default;
  ~ClientGameModel() = default;

  std::shared_ptr<Player> GetLocalPlayer() const;
  GameObjectId GetLocalPlayerId() const;
  std::shared_ptr<PlayerStats> GetLocalPlayerStats();
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);
  void SetLocalPlayerNickname(const QString& nickname);
  QString GetLocalPlayerNickname() const;

  void AddInterpolateInfo(GameObjectId game_object_id,
                          GameObjectType game_object_type,
                          int64_t server_time);

  bool IsGameObjectInInterpolation(GameObjectId game_object_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const;

  const std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
    GetInterpolatorMap() const;

  void RemoveFromInterpolator(GameObjectId game_object_id);

  void AddLocalBullets(int64_t timestamp,
                       const QList<QVariant>& random_bullet_shifts);

  std::vector<std::shared_ptr<Bullet>> GetLocalBullets() const;
  void DeleteLocalBullet(GameObjectId bullet_id);

  const QList<RoomInfo>& GetRoomsInfo() const;
  void SetRoomsInfo(const QList<QVariant>& rooms_info);

  void Clear() override;

 private:
  QString local_player_nickname_{"Player"};
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> interpolator_;
  std::unordered_map<GameObjectId, std::shared_ptr<Bullet>> local_bullets_;
  int bullet_id_to_set_{1};

  QList<RoomInfo> rooms_info_;
};

#endif  // CLIENT_CLIENT_GAME_MODEL_H_
