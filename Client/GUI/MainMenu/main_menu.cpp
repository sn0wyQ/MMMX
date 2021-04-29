#include "main_menu.h"

MainMenu::MainMenu(AbstractClientView* parent)
    : QWidget(parent), parent_(parent) {
  this->setMouseTracking(false);

  start_game_ = new QPushButton(tr("Start Game"), this);
  connect(start_game_,
          SIGNAL(clicked),
          this,
          &MainMenu::OnStartGameButtonClicked);
}

void MainMenu::OnStartGameButtonClicked() {
  parent_->SetWindow(ClientWindowType::kGameView);
}
