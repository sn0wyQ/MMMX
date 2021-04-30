#include "room_settings.h"

RoomSettings::RoomSettings(const QString& name,
                           ClientId owners_client_id,
                           int max_clients,
                           bool is_public)
    : name_(name),
      owners_client_id_(owners_client_id),
      max_clients_(max_clients),
      is_public_(is_public) {
  if (name_ == "") {
    this->SetRandomName();
  }
}

void RoomSettings::SetRandomName() {
  static std::mt19937 gen(QDateTime::currentMSecsSinceEpoch());
  static std::uniform_int_distribution<int> uid_first_part
      (0, kFirstPartsOfTheName.size() - 1);
  int first_part_index = uid_first_part(gen);

  std::uniform_int_distribution<int> uid_preposition
      (0, kFirstPartsOfTheName.at(first_part_index).prepositions.size() - 1);
  int preposition_index = uid_preposition(gen);
  QString preposition =
      kFirstPartsOfTheName.at(first_part_index)
          .prepositions.at(preposition_index);

  std::uniform_int_distribution<int>
      uid_second_part(0, kSecondPartsOfTheName.at(preposition).size() - 1);
  int second_part_index = uid_second_part(gen);

  name_ = kFirstPartsOfTheName.at(first_part_index).first_part
          + preposition
          + kSecondPartsOfTheName.at(preposition).at(second_part_index);
}

int RoomSettings::GetMaxClients() const {
  return max_clients_;
}

void RoomSettings::SetMaxClients(int max_clients) {
  max_clients_ = max_clients;
}
