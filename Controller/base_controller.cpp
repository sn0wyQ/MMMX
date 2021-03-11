#include "base_controller.h"

#include <utility>

BaseController::BaseController() {
  using std::placeholders::_1;
  SetFunctionForEventType(EventType::kAddNewPlayer,
  std::bind(&BaseController::AddNewPlayerEvent, this, _1));
  SetFunctionForEventType(EventType::kClientDisconnected,
   std::bind(&BaseController::ClientDisconnectedEvent, this, _1));
  SetFunctionForEventType(EventType::kEndGame,
   std::bind(&BaseController::EndGameEvent, this, _1));
  SetFunctionForEventType(EventType::kChangedTestCounter,
   std::bind(&BaseController::ChangedTestCounterEvent, this, _1));
  SetFunctionForEventType(EventType::kPressedTestButton,
   std::bind(&BaseController::PressedTestButtonEvent, this, _1));
  SetFunctionForEventType(EventType::kSetClientsPlayerId,
   std::bind(&BaseController::SetClientsPlayerIdEvent, this, _1));
  SetFunctionForEventType(EventType::kSharePlayersInRoomIds,
   std::bind(&BaseController::SharePlayersInRoomIdsEvent, this, _1));
  SetFunctionForEventType(EventType::kStartGame,
   std::bind(&BaseController::StartGameEvent, this, _1));
  SetFunctionForEventType(EventType::kPlayerDisconnected,
   std::bind(&BaseController::PlayerDisconnectedEvent, this, _1));
  SetFunctionForEventType(EventType::kSendEventToClient,
   std::bind(&BaseController::SendEventToClientEvent, this, _1));
  SetFunctionForEventType(EventType::kSendEventToRoom,
   std::bind(&BaseController::SendEventToRoomEvent, this, _1));
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
  qInfo().noquote().nospace() << "[" << GetControllerName() << "] Handling "
    << event;
  function_for_event_[static_cast<uint32_t>(event.GetType())](event);
}

void BaseController::SetFunctionForEventType(
    EventType event_type, const std::function<void(const Event&)>& func) {
  function_for_event_[static_cast<uint32_t>(event_type)] = func;
}

void BaseController::SendEvent(const Event& event) {
  qInfo().noquote().nospace() << "[" << GetControllerName() << "] Sending "
    << event;
}
