#include "main_menu.h"

MainMenu::MainMenu(AbstractClientView* parent, ClientController* controller)
    : QWidget(parent), parent_(parent), controller_(controller) {
  rooms_info_list_ = new RoomsInfoList(this, controller_->GetModel());

  start_game_ = new QPushButton(tr("Start Game"), this);
  connect(start_game_,
          &QPushButton::clicked,
          this,
          &MainMenu::OnStartGameButtonClicked);

  open_settings_ = new QPushButton(tr("Settings"), this);
  connect(open_settings_,
          &QPushButton::clicked,
          this,
          &MainMenu::OnOpenSettingsButtonClicked);

  quit_app_ = new QPushButton(tr("Quit"), this);
  connect(quit_app_,
          &QPushButton::clicked,
          this,
          &MainMenu::OnQuitAppButtonClicked);
  quit_app_->setObjectName("small_btn");
}

void MainMenu::UpdateRoomsInfoList() {
  rooms_info_list_->UpdateRoomsInfoList();
}

void MainMenu::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  rooms_info_list_->setGeometry(width / 10,
                                height / 10,
                                width * 4 / 5,
                                height * 3 / 5);

  start_game_->setGeometry(width * 2 / 5,
                           height * 15 / 20,
                           width / 5,
                           height / 10);

  open_settings_->setGeometry(width * 2 / 5,
                           height * 43 / 50,
                           width / 5,
                           height / 20);

  quit_app_->setGeometry(width * 9 / 20,
                              height * 95 / 100,
                              width / 10,
                              height / 40);
}

void MainMenu::OnStartGameButtonClicked() {
  RoomId selected_room = rooms_info_list_->GetSelectedRoom();
  // TODO(Klim): some check like "if room is OK" etc.
  if (selected_room == Constants::kNullRoomId) {
    return;
  }
  parent_->ConnectToRoom(selected_room);
  parent_->SetWindow(ClientWindowType::kGameView);
  rooms_info_list_->SelectRoom(Constants::kNullRoomId);
}

void MainMenu::OnOpenSettingsButtonClicked() {
  parent_->SetWindow(ClientWindowType::kSettingsWindow);
}

void MainMenu::OnQuitAppButtonClicked() {
  parent_->QuitApp();
}

void MainMenu::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  static QPixmap background(":background.jpg");
  background = background.scaled(width(), height(),
                                 Qt::KeepAspectRatioByExpanding);
  QPoint center_of_widget{width() / 2, height() / 2};
  QRect rect_of_pixmap = background.rect();
  rect_of_pixmap.moveCenter(center_of_widget);
  painter.drawPixmap(rect_of_pixmap.topLeft(), background);
}
