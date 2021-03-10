#include "client_view.h"

ClientView::ClientView(
    std::shared_ptr<ClientController> controller, QWidget* parent)
    : QMainWindow(parent), controller_(std::move(controller)) {
  resize(310, 70);
  setMinimumSize(310, 70);

  connect(this,
          &ClientView::AddNewControlsEvent,
          controller_.get(),
          &ClientController::OnControlsEventReceived);

  connect(controller_.get(),
          &ClientController::UpdateView,
          this,
          &ClientView::OnUpdate);

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
}

void ClientView::OnTestButtonPressed() {
  int random_test_int = QRandomGenerator::global()->bounded(100);
  emit(AddNewControlsEvent(Event(EventType::kPressedTestButton,
                                 controller_->GetModel()->GetOwnersPlayerId(),
                                 random_test_int)));
}

void ClientView::OnUpdate() {
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
  qInfo() << "[VIEW] Updated";
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  Q_UNUSED(paint_event)
}

void ClientView::keyPressEvent(QKeyEvent* key_event) {
  Q_UNUSED(key_event)
}
