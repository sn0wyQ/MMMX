#include "base_controller.h"

BaseController::BaseController() {
  using std::placeholders::_1;
  SetFunctionForEventType(EventType::kClientDisconnected,
    std::bind(&BaseController::ClientDisconnectedEvent, this, _1));
  SetFunctionForEventType(EventType::kEndGame,
    std::bind(&BaseController::EndGameEvent, this, _1));
  SetFunctionForEventType(EventType::kSetPlayerIdToClient,
    std::bind(&BaseController::SetPlayerIdToClient, this, _1));
  SetFunctionForEventType(EventType::kStartGame,
    std::bind(&BaseController::StartGameEvent, this, _1));
  SetFunctionForEventType(EventType::kPlayerDisconnected,
    std::bind(&BaseController::PlayerDisconnectedEvent, this, _1));
  SetFunctionForEventType(EventType::kSendEventToClient,
    std::bind(&BaseController::SendEventToClientEvent, this, _1));
  SetFunctionForEventType(EventType::kSendEventToRoom,
    std::bind(&BaseController::SendEventToRoomEvent, this, _1));
  SetFunctionForEventType(EventType::kSendNickname,
    std::bind(&BaseController::SendNicknameEvent, this, _1));
  SetFunctionForEventType(EventType::kStartGame,
    std::bind(&BaseController::StartGameEvent, this, _1));
  SetFunctionForEventType(EventType::kUpdateVars,
    std::bind(&BaseController::UpdateVarsEvent, this, _1));
  SetFunctionForEventType(EventType::kSetTimeDifference,
    std::bind(&BaseController::SetTimeDifferenceEvent, this, _1));

  // ------------------- GAME EVENTS -------------------
  SetFunctionForEventType(EventType::kAddLocalPlayerGameObject,
    std::bind(&BaseController::AddLocalPlayerGameObjectEvent, this, _1));
  SetFunctionForEventType(EventType::kSendGameInfoToInterpolate,
    std::bind(&BaseController::SendGameInfoToInterpolateEvent, this, _1));
  SetFunctionForEventType(EventType::kUpdateGameObjectData,
    std::bind(&BaseController::UpdateGameObjectDataEvent, this, _1));
  SetFunctionForEventType(EventType::kUpdatePlayersStats,
    std::bind(&BaseController::UpdatePlayersStatsEvent, this, _1));
  SetFunctionForEventType(EventType::kDeleteGameObject,
    std::bind(&BaseController::DeleteGameObjectEvent, this, _1));
  SetFunctionForEventType(EventType::kSendControls,
    std::bind(&BaseController::SendControlsEvent, this, _1));
  SetFunctionForEventType(EventType::kSendPlayerShooting,
    std::bind(&BaseController::SendPlayerShootingEvent, this, _1));
  SetFunctionForEventType(EventType::kUpdateLocalPlayerHealthPoints,
    std::bind(&BaseController::UpdateLocalPlayerHealthPointsEvent, this, _1));
  SetFunctionForEventType(EventType::kPlayerKilledNotification,
    std::bind(&BaseController::PlayerKilledNotificationEvent, this, _1));
  SetFunctionForEventType(EventType::kLocalPlayerDied,
    std::bind(&BaseController::LocalPlayerDiedEvent, this, _1));
  SetFunctionForEventType(EventType::kIncreaseLocalPlayerExperience,
    std::bind(&BaseController::IncreaseLocalPlayerExperienceEvent, this, _1));
  SetFunctionForEventType(EventType::kSendLevelingPoints,
    std::bind(&BaseController::SendLevelingPointsEvent, this, _1));
  SetFunctionForEventType(EventType::kShootFailed,
    std::bind(&BaseController::ShootFailedEvent, this, _1));
  SetFunctionForEventType(EventType::kStartShootingAnimation,
    std::bind(&BaseController::StartShootingAnimationEvent, this, _1));

  connect(&ticker_, &QTimer::timeout, this, &BaseController::Tick);
}

void BaseController::SetFunctionForEventType(
    EventType event_type, const std::function<void(const Event&)>& func) {
  function_for_event_[static_cast<uint32_t>(event_type)] = func;
}

void BaseController::Tick() {
  QElapsedTimer var_timer;
  var_timer.start();

  this->OnTick(last_tick_.restart());

  while (HasEventsToHandle()) {
    this->HandleEvent(events_to_handle_.front());
    events_to_handle_.pop();
  }

  while (HasEventsToSend()) {
    this->SendEvent(events_to_send_.front());
    events_to_send_.pop();
  }

  var_ = var_timer.elapsed();
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

int BaseController::GetVar() const {
  return var_;
}

void BaseController::AddEventToHandle(const Event& event) {
  events_to_handle_.push(event);
}

void BaseController::AddEventToSend(const Event& event) {
  events_to_send_.push(event);
}

void BaseController::HandleEvent(const Event& event) {
  qDebug().noquote().nospace() << "[" << GetControllerName()
                               << "] Handling " << event;
  function_for_event_[static_cast<uint32_t>(event.GetType())](event);
}

void BaseController::LogEvent(const Event& event) const {
  qDebug().noquote().nospace() << "[" << GetControllerName()
                               << "] Sending " << event;
}

int64_t BaseController::GetCurrentServerTime() const {
  return QDateTime::currentMSecsSinceEpoch();
}
