#ifndef GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
#define GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_

#include <memory>
#include <utility>

#include <QKeyEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>
#include <QWidget>

#include "GUI/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"

class SettingsWindow : public QWidget {
  Q_OBJECT

 public:
  SettingsWindow(AbstractClientView* parent, ClientController* controller);

  void keyPressEvent(QKeyEvent* key_event) override;
  void resizeEvent(QResizeEvent* event) override;

  KeyController* GetKeyController() const;

  public Q_SLOTS:
  void OnBackToMainMenuButtonClicked();

 private:
  AbstractClientView* parent_;

  ClientController* controller_;
  std::shared_ptr<ClientGameModel> model_;

  QPushButton* back_to_main_menu_;
  QTextEdit* nickname_edit_;

  KeyController* key_controller_;
};

#endif  // GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
