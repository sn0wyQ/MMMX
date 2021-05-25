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
          &SettingsWindow::BackToMainMenu);

  nickname_edit_ = new QTextEdit(this);
  nickname_edit_->setPlaceholderText(tr("Nickname (default - \"Player\")"));
  nickname_edit_->setText(
      Settings::GetInstance().GetValueByKey<QString>("main/nickname"));

  enable_smooth_fov_checkbox_ = new QCheckBox(this);
  enable_smooth_fov_checkbox_->setText(
      tr("Enable smooth objects disappearing"));
  enable_smooth_fov_checkbox_->setChecked(
      Settings::GetInstance().GetValueByKey<bool>("main/smooth_fov"));
  connect(enable_smooth_fov_checkbox_, &QCheckBox::stateChanged,
          this, &SettingsWindow::OnEnableSmoothFov);

  enable_full_screen_ = new QCheckBox(this);
  enable_full_screen_->setText(tr("Fullscreen"));
  enable_full_screen_->setChecked(
      Settings::GetInstance().GetValueByKey<bool>("main/fullscreen"));
  connect(enable_full_screen_, &QCheckBox::stateChanged,
          this, &SettingsWindow::OnEnableFullScreen);
}

void SettingsWindow::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  back_to_main_menu_->setGeometry(width / 4,
                                  height / 12,
                                  width / 2,
                                  height / 24);

  static const int setting_height = 50;
  static const int offset = 10;
  int setting_width = width * 3 / 4;

  nickname_edit_->setGeometry(width / 8,
                              height / 6,
                              setting_width,
                              setting_height);

  enable_smooth_fov_checkbox_->setGeometry(
      nickname_edit_->x(),
      nickname_edit_->y() + nickname_edit_->height() + offset,
      setting_width,
      setting_height);

  enable_full_screen_->setGeometry(
      enable_smooth_fov_checkbox_->x(),
      enable_smooth_fov_checkbox_->y()
          + enable_smooth_fov_checkbox_->height() + offset,
      setting_width,
      setting_height);
}

void SettingsWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Escape) {
    this->BackToMainMenu();
  }
}

void SettingsWindow::BackToMainMenu() {
  parent_->SetWindow(ClientWindowType::kMainMenu);
  QString nickname_to_set = nickname_edit_->toPlainText().mid(0, 10);
  nickname_edit_->setText(nickname_to_set);
  model_->SetLocalPlayerNickname(nickname_to_set.isEmpty() ? "Player"
                                                           : nickname_to_set);
  Settings::GetInstance().SetValue("main/nickname", nickname_to_set);
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

void SettingsWindow::OnEnableFullScreen() {
  Settings::GetInstance().SetValue("main/fullscreen",
                                   enable_full_screen_->isChecked());
  parent_->ResetScreenState();
}
