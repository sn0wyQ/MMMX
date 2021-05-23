#ifndef CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
#define CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_

#include <memory>
#include <utility>

#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include "Client/client_controller.h"

class SettingsWindow : public QWidget {
  Q_OBJECT

 public:
  SettingsWindow(QWidget* parent, std::shared_ptr<ClientController> controller);

  public Q_SLOTS:
  void OnSetNicknameButtonClicked();

 private:
  std::shared_ptr<ClientController> controller_;

  QPushButton* set_nickname_;
  QTextEdit* nickname_edit_;
};

#endif  // CLIENT_GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_