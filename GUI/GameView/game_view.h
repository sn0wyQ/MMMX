#ifndef GUI_GAMEVIEW_GAME_VIEW_H_
#define GUI_GAMEVIEW_GAME_VIEW_H_

#include <deque>
#include <memory>
#include <utility>

#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QWidget>

#include "GUI/GameView/KillFeed/kill_feed.h"
#include "GUI/GameView/PlayerBar/player_bar.h"
#include "GUI/GameView/ReloadingField/reloading_field.h"
#include "GUI/GameView/RespawnButton/respawn_button.h"
#include "GUI/GameView/StatsTable/stats_table.h"
#include "GUI/GameView/ViewPort/viewport.h"
#include "GUI/abstract_client_view.h"
#include "Client/KeyController/key_controller.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"

class GameView : public QWidget {
 public:
  GameView(AbstractClientView* parent,
           ClientController* controller,
           KeyController* key_controller);

  std::shared_ptr<Converter> GetConverter();

  void Update();

  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  QPointF GetPlayerToCenterOffset() const;
  void AddKillNotification(const QString& killer_name,
                           const QString& victim_name,
                           WeaponType weapon_type);
  void AddRespawnNotification(const QString& player_name);
  void AddPlayerConnectedNotification(const QString& player_name);
  void AddPlayerDisconnectedNotification(const QString& player_name);

  public Q_SLOTS:
  void OnDisconnectButtonClicked();

 private:
  void ProcessRespawnButton();

  AbstractClientView* parent_;
  ClientController* controller_;
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
  QPushButton* disconnect_button_;

  int64_t last_updated_time_{};
  std::deque<int64_t> last_frame_times_{};

  KeyController* key_controller_;
};

#endif  // GUI_GAMEVIEW_GAME_VIEW_H_
