#ifndef GAME_VIEW_H_
#define GAME_VIEW_H_

#include <memory>
#include <utility>

#include <QLabel>
#include <QResizeEvent>
#include <QWidget>

#include "Client/GUI/GameView/KillFeed/kill_feed.h"
#include "Client/GUI/GameView/PlayerBar/player_bar.h"
#include "Client/GUI/GameView/ReloadingField/reloading_field.h"
#include "Client/GUI/GameView/RespawnButton/respawn_button.h"
#include "Client/GUI/GameView/StatsTable/stats_table.h"
#include "Client/GUI/GameView/ViewPort/viewport.h"
#include "Client/GUI/abstract_client_view.h"
#include "Client/KeyController/key_controller.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"

class GameView : public QWidget {
 public:
  GameView(AbstractClientView* parent,
           std::shared_ptr<ClientController> controller,
           std::shared_ptr<KeyController> key_controller);

  std::shared_ptr<Converter> GetConverter();

  void Update();

  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  QPointF GetPlayerToCenterOffset() const;
  void AddKillFeedNotification(const QString& killer_name,
                               const QString& victim_name,
                               WeaponType weapon_type);
  void AddRespawnNotification(const QString& player_name);
  void AddPlayerConnectedNotification(const QString& player_name);
  void AddPlayerDisconnectedNotification(const QString& player_name);

 private:
  void ProcessRespawnButton();

  AbstractClientView* parent_;
  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<ClientGameModel> model_;

  KillFeed* kill_feed_;
  PlayerBar* player_bar_;
  int height_of_bar_{};
  ReloadingField* reloading_field_;
  RespawnButton* respawn_button_;
  QPoint respawn_button_default_position_;
  StatsTable* stats_table_;
  QLabel* info_label_;
  ViewPort* view_port_;

  int64_t last_updated_time_{};
  std::deque<int64_t> last_frame_times_{};

  std::shared_ptr<KeyController> key_controller_;
};

#endif  // GAME_VIEW_H
