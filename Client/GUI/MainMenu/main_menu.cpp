#include "main_menu.h"

MainMenu::MainMenu(AbstractClientView* parent,
                   std::shared_ptr<ClientController> controller)
    : QWidget(parent), parent_(parent), controller_(std::move(controller)) {
  this->setMouseTracking(false);

  rooms_info_list_ = new RoomsInfoList(this, controller_->GetModel());

  start_game_ = new QPushButton(tr("Start Game"), this);
  connect(start_game_,
          &QPushButton::clicked,
          this,
          &MainMenu::OnStartGameButtonClicked);
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
                           height * 4 / 5,
                           width / 5,
                           height / 10);
}

void MainMenu::OnStartGameButtonClicked() {
  // TODO(Klim): some check like "if room is OK" etc.
  parent_->ConnectToRoom(rooms_info_list_->GetSelectedRoom());
  parent_->SetWindow(ClientWindowType::kGameView);
}
