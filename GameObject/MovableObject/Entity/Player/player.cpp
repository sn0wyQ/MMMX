#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {
  this->SetId(player_id);
}

void Player::ChangeTestCounter(int delta) {
  test_counter_ += delta;
}

int Player::GetTestCounterValue() const {
  return test_counter_;
}

void Player::SetTestCounterValue(int value) {
  test_counter_ = value;
}

int Player::GetTestCounterLastDeltaValue() const {
  return test_counter_last_delta_;
}

void Player::SetTestCounterLastDeltaValue(int value) {
  test_counter_last_delta_ = value;
}

void Player::ApplyDirection(uint32_t mask) {
  QPoint pos = GetPosition();
  if ((mask >> 3u) & 1u) {
    pos.setY(pos.y() - 1);
  }
  if ((mask >> 2u) & 1u) {
    pos.setX(pos.x() + 1);
  }
  if ((mask >> 1u) & 1u) {
    pos.setY(pos.y() + 1);
  }
  if ((mask >> 0u) & 1u) {
    pos.setX(pos.x() - 1);
  }
  SetPosition(pos);
}

Player::Player(GameObjectId player_id, int x, int y)
  : Player(player_id) {
  SetX(x);
  SetY(y);
}
