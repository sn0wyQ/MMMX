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

  enable_smooth_fov_checkbox_ = new QCheckBox(this);
  enable_smooth_fov_checkbox_->setText(
      tr("Enable smooth objects disappearing"));
  connect(enable_smooth_fov_checkbox_, &QCheckBox::stateChanged,
          this, &SettingsWindow::OnEnableSmoothFov);
}

void SettingsWindow::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  back_to_main_menu_->setGeometry(width / 4,
                                  height / 12,
                                  width / 2,
                                  height / 24);

  int setting_height = 50;
  int setting_width = width * 3 / 4;
  int offset = 10;

  nickname_edit_->setGeometry(width / 8,
                              height / 6,
                              setting_width,
                              setting_height);

  enable_smooth_fov_checkbox_->setGeometry(
      nickname_edit_->x(),
      nickname_edit_->y() + nickname_edit_->height() + offset,
      setting_width,
      setting_height);
}

void SettingsWindow::OnBackToMainMenuButtonClicked() {
  parent_->SetWindow(ClientWindowType::kMainMenu);
  QString nickname_to_set = nickname_edit_->toPlainText();
  model_->SetLocalPlayerNickname(nickname_to_set.isEmpty() ? "Player"
                                                           : nickname_to_set);
}

void SettingsWindow::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  static QPixmap background(":background.jpg");
  background = background.scaled(width(), height(),
                                 Qt::KeepAspectRatioByExpanding);
  QPoint center_of_widget{width() / 2, height() / 2};
  QRect rect_of_pixmap = background.rect();
  rect_of_pixmap.moveCenter(center_of_widget);
  painter.drawPixmap(rect_of_pixmap.topLeft(), background);
}

void SettingsWindow::OnEnableSmoothFov() {
  Settings::GetInstance().SetValue("main/smooth_fov",
                                   enable_smooth_fov_checkbox_->isChecked());
}
