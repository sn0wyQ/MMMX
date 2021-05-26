#ifndef GUI_CLIENT_VIEW_H_
#define GUI_CLIENT_VIEW_H_

#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include <QCloseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>

#include "GUI/GameView/game_view.h"
#include "GUI/MainMenu/main_menu.h"
#include "GUI/SettingsWindow/settings_window.h"
#include "abstract_client_view.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"
#include "Event/event.h"

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(ClientController* controller);
  ~ClientView() override = default;

  void ConnectToRoom(RoomId room_id, PlayerType player_type) override;
  void SetWindow(ClientWindowType window_type) override;
  void Update() override;
  void UpdateRoomsInfoList() override;
  std::shared_ptr<Converter> GetConverter() override;
  QPointF GetPlayerToCenterOffset() const override;
  void AddKillFeedNotification(const QString& killer_name,
                               const QString& victim_name,
                               WeaponType weapon_type) override;

  KeyController* GetKeyController() const override;
  void AddRespawnNotification(const QString& player_name) override;
  void AddPlayerConnectedNotification(const QString& player_name) override;
  void AddPlayerDisconnectedNotification(const QString& player_name) override;
  void QuitApp() override;

  void SetFullScreen();
  void SetWindowed();
  void ResetScreenState() override;

 private:
  void closeEvent(QCloseEvent* close_event) override;
  void focusOutEvent(QFocusEvent* focus_event) override;
  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void mousePressEvent(QMouseEvent* mouse_event) override;
  void mouseReleaseEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

  ClientController* controller_;
  std::shared_ptr<ClientGameModel> model_;

  QStackedWidget* stacked_widget_;
  GameView* game_view_;
  MainMenu* main_menu_;
  SettingsWindow* settings_window_;

  KeyController* key_controller_;
};

#endif  // GUI_CLIENT_VIEW_H_
