#include "client_view.h"

ClientView::ClientView(
    std::shared_ptr<ClientController> controller)
    : AbstractClientView(), controller_(std::move(controller)),
    timer_for_keys_(new QTimer()) {
  resize(700, 700);
  setMinimumSize(310, 70);
  setWindowTitle(Constants::kWindowTitle);

  controller_->SetView(std::shared_ptr<ClientView>(this));

  test_button_ = new QPushButton("Waiting for connection...", this);
  test_button_->setEnabled(false);
  test_button_->setGeometry(0, 0, 310, 25);
  connect(test_button_,
          &QPushButton::pressed,
          this,
          &ClientView::OnTestButtonPressed);

  my_counter_label_ = new QLabel("My Counter: 0", this);
  my_counter_label_->setGeometry(0, 30, 150, 30);

  enemy_counter_label_ = new QLabel("Enemy Counter: 0", this);
  enemy_counter_label_->setGeometry(160, 30, 150, 30);

  connect(timer_for_keys_, &QTimer::timeout, this,
          &ClientView::ApplyDirection);
  timer_for_keys_->start(5);
}

void ClientView::OnTestButtonPressed() {
  int random_test_int = QRandomGenerator::global()->bounded(100);
  controller_->ReceiveEvent(
      Event(EventType::kPressedTestButton,
            controller_->GetModel()->GetOwnersPlayerId(),
            random_test_int));
}

void ClientView::Update() {
  if (controller_->IsInProgress()) {
    test_button_->setText("Counter Add Random Number");
    test_button_->setEnabled(true);


    auto owners_player_ptr = controller_->GetModel()->GetOwnersPlayer();
    my_counter_label_->setText(QStringLiteral("My Counter: %1 (+%2)")
                   .arg(owners_player_ptr->GetTestCounterValue())
                   .arg(owners_player_ptr->GetTestCounterLastDeltaValue()));

    auto enemy_player_ptr = controller_->GetModel()->GetTestEnemyPlayer();
    enemy_counter_label_->setText(QStringLiteral("Enemy Counter: %1 (+%2)")
                   .arg(enemy_player_ptr->GetTestCounterValue())
                   .arg(enemy_player_ptr->GetTestCounterLastDeltaValue()));
  } else {
    test_button_->setText("Waiting for connection...");
    test_button_->setEnabled(false);
  }

  this->update();
  repaint();
  // qInfo().noquote().nospace() << "[VIEW] Updated";
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  QPainter painter(this);
  std::vector<std::shared_ptr<Player>> players
    = controller_->GetModel()->GetPlayers();
  for (const auto& player : players) {
    painter.drawEllipse(player->GetPosition(), 50, 50);
  }
}

void ClientView::keyPressEvent(QKeyEvent* key_event) {
  is_direction_by_keys_[key_to_direction_[key_event->nativeVirtualKey()]] = true;
}

void ClientView::keyReleaseEvent(QKeyEvent* key_event) {
  is_direction_by_keys_[key_to_direction_[key_event->nativeVirtualKey()]] = false;
}

void ClientView::ApplyDirection() {
  is_direction_applied_[Direction::kUp]
  = is_direction_applied_[Direction::kRight]
      = is_direction_applied_[Direction::kDown]
          = is_direction_applied_[Direction::kLeft] = false;
  bool is_up_pressed
    = is_direction_by_keys_[Direction::kUp];
  bool is_right_pressed
      = is_direction_by_keys_[Direction::kRight];
  bool is_down_pressed
      = is_direction_by_keys_[Direction::kDown];
  bool is_left_pressed
      = is_direction_by_keys_[Direction::kLeft];
  if ((is_up_pressed ^ is_down_pressed) == 1) {
    is_direction_applied_[is_up_pressed ? Direction::kUp : Direction::kDown] = true;
  }
  if ((is_right_pressed ^ is_left_pressed) == 1) {
    is_direction_applied_[is_right_pressed ? Direction::kRight : Direction::kLeft] = true;
  }

  int mask = is_direction_applied_[Direction::kUp] * 8
      + is_direction_applied_[Direction::kRight] * 4
      + is_direction_applied_[Direction::kDown] * 2
      + is_direction_applied_[Direction::kLeft];
  if (mask != 0) {
    controller_->ReceiveEvent(
        Event(EventType::kSendDirectionInfo,
              controller_->GetModel()->GetOwnersPlayerId(),
              mask));
  }
  // qInfo() << is_direction_applied_[Direction::kUp]
  // << is_direction_applied_[Direction::kRight]
  // << is_direction_applied_[Direction::kDown]
  // << is_direction_applied_[Direction::kLeft];
}
