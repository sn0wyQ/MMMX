#include "base_controller.h"

#include <utility>

BaseController::BaseController() {
  using std::placeholders::_1;
  function_for_event_ = {
      {EventType::kAddNewPlayer,
       std::bind(&BaseController::AddNewPlayerEvent, this, _1)},
      {EventType::kClientDisconnected,
       std::bind(&BaseController::ClientDisconnectedEvent, this, _1)},
      {EventType::kEndGame,
       std::bind(&BaseController::EndGameEvent, this, _1)},
      {EventType::kChangedTestCounter,
       std::bind(&BaseController::ChangedTestCounterEvent, this, _1)},
      {EventType::kPressedTestButton,
       std::bind(&BaseController::PressedTestButtonEvent, this, _1)},
      {EventType::kSetClientsPlayerId,
       std::bind(&BaseController::SetClientsPlayerIdEvent, this, _1)},
      {EventType::kSharePlayersInRoomIds,
       std::bind(&BaseController::SharePlayersInRoomIdsEvent,
                 this, _1)},
      {EventType::kStartGame,
       std::bind(&BaseController::StartGameEvent, this, _1)},
      {EventType::kPlayerDisconnected,
       std::bind(&BaseController::PlayerDisconnectedEvent, this, _1)},
      {EventType::kSendEventToClient,
       std::bind(&BaseController::AddEventToSend, this, _1)},
      {EventType::kSendEventToRoom,
       std::bind(&BaseController::AddEventToSend, this, _1)},
      {EventType::kSetClientsPlayerId,
       std::bind(&BaseController::AddEventToSend, this, _1)},
  };
  connect(&ticker_,
          &QTimer::timeout,
          this,
          &BaseController::Tick);
}

void BaseController::Tick() {
  // ЭТО ХЕНДНДЛ ТУТ ПРОСТО МЯСОДРОБИЛКА ХЕНДЛОВ (с) @watislaf
  while (HasEventsToHandle()) {
    this->HandleEvent(events_to_handle_.front());
    events_to_handle_.pop();
  }
  // А ТУТ СЕНД ТУТ ВСЕ НА СЕНД (с) @watislaf
  while (HasEventsToSend()) {
    this->SendEvent(events_to_send_.front());
    events_to_send_.pop();
  }
  this->OnTick();
}

bool BaseController::HasEventsToSend() const {
  return !events_to_send_.empty();
}

bool BaseController::HasEventsToHandle() const {
  return !events_to_handle_.empty();
}

void BaseController::StartTicking() {
  ticker_.start(Constants::kTimeToTick);
  last_tick_.start();
}

void BaseController::AddEventToHandle(const Event& event) {
  events_to_handle_.push(event);
}

void BaseController::AddEventToSend(const Event& event) {
  events_to_send_.push(event);
}

void BaseController::HandleEvent(const Event& event) {
  function_for_event_.at(event.GetType())(event);
}

void BaseController::SetFunctionForEvent(
    const Event& event, std::function<void(const Event&)> func) {
  function_for_event_[event.GetType()] = std::move(func);
}

