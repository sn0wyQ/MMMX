#include "base_controller.h"

BaseController::BaseController() {
  connect(&ticker_,
          &QTimer::timeout,
          this,
          &BaseController::Tick);
}

void BaseController::Tick() {
  while (!events_to_handle_.empty()) {
    this->HandleEvent(events_to_handle_.front());
    events_to_handle_.pop();
  }
  Send();
}

bool BaseController::HasEventsToSend() {
  return !events_to_send_.empty();
}

Event BaseController::GetNextEventToSend() {
  Event next_event = events_to_send_.front();
  events_to_send_.pop();
  return next_event;
}

void BaseController::StartTicking() {
  ticker_.start(Constants::kTimeToTick);
  last_tick_.start();
}

void BaseController::StopTicking() {
  ticker_.stop();
  last_tick_.invalidate();
}
