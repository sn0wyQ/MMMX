#ifndef GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
#define GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_

#include <memory>
#include <utility>

#include <QCheckBox>
#include <QComboBox>
#include <QListView>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>
#include <QWidget>

#include "Client/client_controller.h"
#include "Client/client_game_model.h"
#include "GUI/abstract_client_view.h"

class SettingsWindow : public QWidget {
  Q_OBJECT

 public:
  SettingsWindow(AbstractClientView* parent, ClientController* controller);

  void keyPressEvent(QKeyEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void paintEvent(QPaintEvent*) override;

  void ResetNickname();

  public Q_SLOTS:
  void BackToMainMenu();
  void OnEnableSmoothFov();
  void OnEnableFullScreen();
  void OnSetAntialiasing(int new_index);

 private:
  AbstractClientView* parent_;

  ClientController* controller_;
  std::shared_ptr<ClientGameModel> model_;

  QPushButton* back_to_main_menu_;
  QTextEdit* nickname_edit_;
  QCheckBox* enable_smooth_fov_checkbox_;
  QCheckBox* enable_full_screen_;
  QComboBox* antialiasing_types_;
};

#endif  // GUI_SETTINGSWINDOW_SETTINGS_WINDOW_H_
