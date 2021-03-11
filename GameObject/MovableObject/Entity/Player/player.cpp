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
