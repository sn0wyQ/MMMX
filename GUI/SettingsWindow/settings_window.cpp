#include "settings_window.h"

SettingsWindow::SettingsWindow(AbstractClientView* parent,
                               ClientController* controller)
    : parent_(parent),
      controller_(controller),
      model_(controller_->GetModel()) {
  back_to_main_menu_ = new QPushButton(tr("Main Menu"), this);
  connect(back_to_main_menu_,
          &QPushButton::clicked,
          this,
          &SettingsWindow::OnBackToMainMenuButtonClicked);

  nickname_edit_ = new QTextEdit(this);
  nickname_edit_->setPlaceholderText(tr("Nickname (default - \"Player\")"));

  set_nickname_ = new QPushButton(tr("Set nickname"), this);
  connect(set_nickname_,
          &QPushButton::clicked,
          this,
          &SettingsWindow::OnSetNicknameButtonClicked);
}

void SettingsWindow::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  back_to_main_menu_->setGeometry(width / 4,
                                  height / 12,
                                  width / 2,
                                  height / 24);

  nickname_edit_->setGeometry(width / 8,
                              height / 6,
                              width * 3 / 4,
                              height * 2 / 3);

  set_nickname_->setGeometry(width / 4,
                             height * 11 / 12,
                             width / 2,
                             height / 24);
}

void SettingsWindow::OnBackToMainMenuButtonClicked() {
  parent_->SetWindow(ClientWindowType::kMainMenu);
}

void SettingsWindow::OnSetNicknameButtonClicked() {
  QString nickname_to_set = nickname_edit_->toPlainText();
  model_->SetLocalPlayerNickname(nickname_to_set.isEmpty() ? "Player"
                                                           : nickname_to_set);
}
