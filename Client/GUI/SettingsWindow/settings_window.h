#ifndef CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
#define CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_

#include <memory>
#include <utility>

#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>
#include <QWidget>

#include "Client/GUI/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"

class SettingsWindow : public QWidget {
  Q_OBJECT

 public:
  SettingsWindow(AbstractClientView* parent,
                 std::shared_ptr<ClientController> controller);

  void resizeEvent(QResizeEvent* event) override;

  public Q_SLOTS:
  void OnBackToMainMenuButtonClicked();
  void OnSetNicknameButtonClicked();

 private:
  AbstractClientView* parent_;

  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<ClientGameModel> model_;

  QPushButton* back_to_main_menu_;
  QPushButton* set_nickname_;
  QTextEdit* nickname_edit_;
};

#endif  // CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
