#include "room_settings.h"

std::mt19937 RoomSettings::gen_(QDateTime::currentMSecsSinceEpoch());

RoomSettings::RoomSettings(QString name,
                           ClientId owners_client_id,
                           int max_clients,
                           bool is_public)
    : name_(std::move(name)),
      owners_client_id_(owners_client_id),
      max_clients_(max_clients),
      is_public_(is_public) {
  if (name_ == "") {
    this->SetRandomName();
  }

  this->SetRandomKeyCode();
}

QString RoomSettings::GetName() const {
  return name_;
}

void RoomSettings::SetName(const QString& name) {
  name_ = name;
}

void RoomSettings::SetRandomName() {
  static std::uniform_int_distribution<int> uid_first_part
      (0, kFirstPartsOfTheName.size() - 1);
  int first_part_index = uid_first_part(gen_);

  std::uniform_int_distribution<int> uid_preposition
      (0, kFirstPartsOfTheName.at(first_part_index).prepositions.size() - 1);
  int preposition_index = uid_preposition(gen_);
  QString preposition =
      kFirstPartsOfTheName.at(first_part_index)
          .prepositions.at(preposition_index);

  std::uniform_int_distribution<int>
      uid_second_part(0, kSecondPartsOfTheName.at(preposition).size() - 1);
  int second_part_index = uid_second_part(gen_);

  name_ = kFirstPartsOfTheName.at(first_part_index).first_part
          + " " + preposition + " "
          + kSecondPartsOfTheName.at(preposition).at(second_part_index);
}

ClientId RoomSettings::GetOwnersClientId() const {
  return owners_client_id_;
}

void RoomSettings::SetOwnersClientId(ClientId new_owner_id) {
  owners_client_id_ = new_owner_id;
}

bool RoomSettings::IsPublic() const {
  return is_public_;
}

void RoomSettings::SetPublicity(bool is_public) {
  is_public_ = is_public;
}

int RoomSettings::GetMaxClients() const {
  return max_clients_;
}

void RoomSettings::SetMaxClients(int max_clients) {
  max_clients_ = max_clients;
}

QString RoomSettings::GetKeyCode() const {
  return key_code_;
}

void RoomSettings::SetRandomKeyCode() {
  static auto RandomChar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::uniform_int_distribution<int> uid_char(0, sizeof(charset) - 1);
    return charset[uid_char(gen_)];
  };

  key_code_.resize(Constants::RoomSettings::kKeyCodeLength);
  std::generate_n(key_code_.begin(),
                  Constants::RoomSettings::kKeyCodeLength,
                  RandomChar);
}
