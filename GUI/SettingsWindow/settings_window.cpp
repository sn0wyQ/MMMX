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
  nickname_edit_->setFixedHeight(50);

  key_controller_ = new KeyController(this);
  key_controller_->Hide();
}

void SettingsWindow::keyPressEvent(QKeyEvent* key_event) {
  if (key_controller_->IsShown()) {
    if (key_event->key() == Qt::Key_F1 || key_event->key() == Qt::Key_Escape) {
      key_controller_->Hide();
    }
  } else if (key_event->key() == Qt::Key_F1) {
    key_controller_->Show();
  }
  if (key_controller_->IsShown()) {
    key_controller_->keyPressEvent(key_event);
  }
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

  key_controller_->move(width / 4, height / 4);
  key_controller_->resize(width / 2, height);
}

KeyController* SettingsWindow::GetKeyController() const {
  return key_controller_;
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
