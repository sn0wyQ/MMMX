#include "settings_window.h"

SettingsWindow::SettingsWindow(QWidget* parent,
                               std::shared_ptr<ClientController> controller)
    : controller_(std::move(controller)) {
  nickname_edit_ = new QTextEdit(this);
  nickname_edit_->setPlaceholderText("Nickname (default - \"Player\")");
  connect(set_nickname_,
          &QPushButton::clicked,
          this,
          &SettingsWindow::OnSetNicknameButtonClicked);
}

QString SettingsWindow::GetNickname() const {
  return nickname_;
}

void SettingsWindow::OnSetNicknameButtonClicked() {
  nickname_ = nickname_edit_->toPlainText();
}
