#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url),
  model_(std::make_shared<GameDataModel>()) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  connect(&web_socket_, &QWebSocket::connected, this,
          &ClientController::OnConnected);
  connect(&web_socket_, &QWebSocket::disconnected, this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  this->StartTicking();
}

std::shared_ptr<GameDataModel> ClientController::GetModel() {
  return model_;
}

bool ClientController::IsGameInProgress() const {
  return game_state_ == GameState::kGameInProgress;
}

void ClientController::OnConnected() {
  connect(&web_socket_,
          &QWebSocket::binaryMessageReceived,
          this,
          &ClientController::OnByteArrayReceived);

  connect(&timer_for_server_var_, &QTimer::timeout, this,
          &ClientController::UpdateServerVar);
  timer_for_server_var_.start(Constants::kTimeToUpdateServerVar);
  timer_elapsed_server_var_.start();
  connect(&web_socket_, &QWebSocket::pong, this,
          &ClientController::UpdatePing);

  view_->Update();

  // TODO(Everyone): Send nickname to server after connection

  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::AddNewPlayerEvent(const Event& event) {
  model_->AddPlayer(event.GetArg<GameObjectId>(1));
  view_->Update();
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kFinished;
  view_->Update();
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_->SetLocalPlayerId(event.GetArg<GameObjectId>(1));
  qInfo().noquote() << "[CLIENT] Set player_id to"
                    << event.GetArg<GameObjectId>(1);
}

void ClientController::CreateAllPlayersDataEvent(const Event& event) {
  for (int i = 2; i < event.GetArg<int>(1) * 4 + 2; i += 4) {
    model_->AddPlayer(event.GetArg<GameObjectId>(i),
                     event.GetArg<float>(i + 1),
                     event.GetArg<float>(i + 2),
                     event.GetArg<float>(i + 3));
  }
  view_->Update();
}

void ClientController::StartGameEvent(const Event& event) {
  game_state_ = GameState::kGameInProgress;
  view_->Update();
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::CollidePlayerWithGameObjects(
    int time_from_previous_tick) {
  auto local_player = model_->GetLocalPlayer();
  QVector2D key_force = GetKeyForce();

  if (!key_force.isNull()) {
    local_player->SetVelocity(key_force);

    // Первая фаза (прижимная): если в след тик будем в объекте,
    // то делаем такую скорость, чтобы мы в след тик были прижатыми
    // к этому объекту
    for (const auto& item : model_->GetAllGameObjects()) {
      if (local_player->GetId() == item->GetId()) {
        continue;
      }

      QVector2D offset = QVector2D(item->GetX() - local_player->GetX(),
                                   item->GetY() - local_player->GetY());
      float rotation = item->GetRotation();
      std::vector<QPointF> intersect_points_now
          = IntersectChecker::GetIntersectPointsBodies(
              local_player->GetRigidBody(), item->GetRigidBody(),
              offset, rotation);

      std::vector<QPointF> intersect_points_in_future
          = IntersectChecker::GetIntersectPointsBodies(
              local_player->GetRigidBody(), item->GetRigidBody(),
              offset
                  - local_player->GetAppliedDeltaPosition(time_from_previous_tick),
              rotation);

      // Если мы будем в объекте в будующем...
      if (!intersect_points_in_future.empty()) {

        // То дихаем такое расстояние, чтобы не быть в нем
        QVector2D delta_to_set
            = IntersectChecker::CalculateDistanceToObjectNotToIntersectBodies(
                local_player->GetRigidBody(), item->GetRigidBody(),
                offset, rotation,
                local_player->GetAppliedDeltaPosition(time_from_previous_tick));
        // С учетом текущей скорости и тика переводим расстояние в скорость
        QVector2D velocity_to_set = local_player->GetVelocityByDeltaPosition(
            delta_to_set, time_from_previous_tick);

        // Обновляем скорость
        local_player->SetVelocity(velocity_to_set);
      }
    }

    // Вторая фаза (скользкая):
    // Если наша внешняя оболочка [RigidBody::External()]
    // оказалась уже прижата,
    // то ищем все касательные к нашему объекту
    std::vector<QVector2D> tangents;
    if (local_player->GetVelocity().isNull()) {
      for (const auto& item : model_->GetAllGameObjects()) {
        if (local_player->GetId() == item->GetId()) {
          continue;
        }

        QVector2D offset = QVector2D(item->GetX() - local_player->GetX(),
                                     item->GetY() - local_player->GetY());
        float rotation = item->GetRotation();
        std::vector<QPointF> intersect_points_now
            = IntersectChecker::GetIntersectPointsBodies(
                local_player->GetRigidBody()->External(), item->GetRigidBody(),
                offset, rotation);

        for (const auto& point : intersect_points_now) {
          // Из вектора, задающего точку прикосновения
          // делаем касательную
          QVector2D tangent_vector(-point.y(), point.x());
          tangent_vector.normalize();
          tangents.push_back(tangent_vector);
        }
      }
    }

    // Если касательные нашлись, то пробуем скользить по ним
    if (!tangents.empty()) {
      bool full_stop = false;
      QVector2D common_tangent_vector;
      bool has_common = false;

      for (const auto& tangent_vector : tangents) {
        // cos = косинус угла между касательной и нажатием клавиш
        // также он будет равен будущей скорости, т.к. есть зависимость
        // чем острее угол - тем больше скалярное произведение
        float cos = QVector2D::dotProduct(
            key_force, QVector2D(tangent_vector.y(), -tangent_vector.x()));
        // Это проверка на то, находится ли касательная на пути нажатия клавиш
        if (cos <= 0) {
          continue;
        }
        if (!has_common) {
          has_common = true;
          common_tangent_vector = tangent_vector;
          continue;
        }

        // Если мы нашли несколько касательных, то тут два варианта
        // - они как бы замыкают нас и довольно строго, что дальше пройти
        // мы не сможем никак
        // - они более-менее похожи, тогда мы можем проскальзить по двоим сразу,
        // тогда найдем сумму, а потом ее нормализуем
        if (IntersectChecker::IsSimilarVectors(common_tangent_vector,
                                               tangent_vector)) {
          common_tangent_vector += tangent_vector;
          continue;
        }
        // Если касательные не похожи, то мы должны остановится
        full_stop = true;
        break;
      }
      if (full_stop) {
        local_player->SetVelocity({0.f, 0.f});
      } else {
        float length_result;
        if (has_common) {
          common_tangent_vector.normalize();
          // Теперь окончательно посчитаем скорость
          length_result = QVector2D::dotProduct(common_tangent_vector,
                                                key_force);
        } else {
          // Если все наши касательные нам не мешают,
          // то мы можем просто применить движение туда,
          // куда нажали клавиши
          length_result = 1.f;
          common_tangent_vector = key_force;
        }
        common_tangent_vector.normalize();

        QVector2D result = common_tangent_vector * length_result;
        local_player->SetVelocity(result);
      }
    }
  }
}

void ClientController::OnTick(int time_from_previous_tick) {
  if (model_->IsLocalPlayerSet()) {
    CollidePlayerWithGameObjects(time_from_previous_tick);
  }

  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->OnTick(time_from_previous_tick);
  }

  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();

  converter_->UpdateGameCenter(local_player->GetPosition());

  this->AddEventToHandle(Event(EventType::kSendControls,
                               local_player->GetId(),
                               local_player->GetX(),
                               local_player->GetY(),
                               local_player->GetVelocity(),
                               local_player->GetRotation()));

  view_->Update();
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_->DeletePlayer(event.GetArg<GameObjectId>(0));
  game_state_ = GameState::kNotStarted;
  view_->Update();
}

void ClientController::SetView(std::shared_ptr<AbstractClientView> view) {
  view_ = std::move(view);
  converter_ = view_->GetConverter();
}

QString ClientController::GetControllerName() const {
  return "CLIENT";
}

int ClientController::GetPing() const {
  return ping_;
}

int ClientController::GetServerVar() const {
  return server_var_;
}

void ClientController::UpdatePing(int elapsed_time) {
  ping_ = elapsed_time;
}

void ClientController::UpdateServerVar() {
  this->AddEventToSend(Event(EventType::kUpdateServerVar,
                             model_->GetLocalPlayerId()));
  timer_elapsed_server_var_.restart();
  web_socket_.ping();
}

void ClientController::UpdateServerVarEvent(const Event& event) {
  server_var_ = static_cast<int>(timer_elapsed_server_var_.elapsed()) / 2;
  view_->Update();
}

// ------------------- GAME EVENTS -------------------

void ClientController::SendControlsEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ClientController::UpdatePlayerDataEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto player_ptr = model_->GetPlayerByPlayerId(event.GetArg<GameObjectId>(0));

  if (player_ptr->IsLocalPlayer()) {
    return;
  }

  player_ptr->SetX(event.GetArg<float>(1));
  player_ptr->SetY(event.GetArg<float>(2));
  player_ptr->SetVelocity(event.GetArg<QVector2D>(3));
  player_ptr->SetRotation(event.GetArg<float>(4));

  view_->Update();
}

void ClientController::GameObjectAppearedEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto game_object_id = event.GetArg<GameObjectId>(1);
  auto game_object_type
      = static_cast<GameObjectType>(event.GetArg<int>(2));
  switch (game_object_type) {
    case GameObjectType::kBox:
      model_->AddBox(game_object_id,
                         event.GetArg<float>(3),
                         event.GetArg<float>(4),
                         event.GetArg<float>(5),
                         event.GetArg<float>(6),
                          event.GetArg<float>(7));
      break;
    case GameObjectType::kTree:
      model_->AddTree(game_object_id,
                     event.GetArg<float>(3),
                     event.GetArg<float>(4),
                     event.GetArg<float>(5));
      break;
  }
  view_->Update();
}

// -------------------- CONTROLS --------------------

void ClientController::FocusOutEvent(QFocusEvent* focus_event) {
  for (const auto& [key, direction] : key_to_direction_) {
    is_direction_by_keys_[direction] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity({0, 0});
  }
}

void ClientController::KeyPressEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = true;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {
  if (model_->IsLocalPlayerSet()) {
    auto local_player = model_->GetLocalPlayer();
    float rotation = Math::DirectionAngle(local_player->GetPosition(),
                                            converter_->PointFromScreenToGame(
                                                mouse_event->pos()));
    local_player->SetRotation(rotation);
    view_->Update();
  }
}

QVector2D ClientController::GetKeyForce() const {
  bool is_up_pressed = is_direction_by_keys_.at(Direction::kUp);
  bool is_right_pressed = is_direction_by_keys_.at(Direction::kRight);
  bool is_down_pressed = is_direction_by_keys_.at(Direction::kDown);
  bool is_left_pressed = is_direction_by_keys_.at(Direction::kLeft);

  QVector2D key_force;
  if ((is_up_pressed ^ is_down_pressed) == 1) {
    if (is_up_pressed) {
      key_force.setY(-1.f);
    } else {
      key_force.setY(1.f);
    }
  }
  if ((is_right_pressed ^ is_left_pressed) == 1) {
    if (is_right_pressed) {
      key_force.setX(1.f);
    } else {
      key_force.setX(-1.f);
    }
  }

  key_force.normalize();
  return key_force;
}
