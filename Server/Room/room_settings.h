#ifndef SERVER_ROOM_ROOM_SETTINGS_H_
#define SERVER_ROOM_ROOM_SETTINGS_H_

#include <algorithm>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QDateTime>
#include <QDebug>

#include "constants.h"

namespace Constants::RoomSettings {

constexpr int kKeyCodeLength = 5;

}  // namespace Constants::RoomSettings

struct FirstPartOfTheName {
  // Should be somewhat close to 'battle' (by sense)
  QString first_part;
  // Accepted prepositions for the second part
  std::vector<QString> prepositions;
};
const std::vector<FirstPartOfTheName> kFirstPartsOfTheName{
    {"Battle",
     {"at", "for the", "in the", "of", "of the"}},
    {"Bloodbath", {"at", "in the"}},
    {"Fight",
     {"at", "for the", "in the", "of", "of the"}},
    {"Massacre", {"at", "in the"}},
    {"Skirmish", {"at", "in the"}},

};

// [preposition, second_part]
const std::unordered_map<QString, std::vector<QString>> kSecondPartsOfTheName {
    {"at", {"Cory's", "Garden's Gate", "Kemble's Cascade", "Mory's", "Tory's"}},
    {"for the", {"King's Crown", "Satan's Soul", "Throne", "Unlimited Power"}},
    {"in the", {"Flowery Gardens", "Greenwood Forest", "Evening Sunshine"}},
    {"of", {"Animals", "Champions", "Marathon", "Hastings", "Tours"}},
    {"of the", {"Baggage", "Lost Time", "Mermaid Lake", "Night Power", "Pit"}},
};

class RoomSettings {
 public:
  explicit RoomSettings(QString name = "",
                        ClientId owners_client_id = Constants::kNullClientId,
                        int max_clients = Constants::kDefaultMaxClients,
                        bool is_public = true);

  QString GetName() const;
  void SetName(const QString& name);
  void SetRandomName();

  ClientId GetOwnersClientId() const;
  void SetOwnersClientId(ClientId new_owner_id);

  int GetMaxClients() const;
  void SetMaxClients(int max_clients);

  bool IsPublic() const;
  void SetPublicity(bool is_public);

  QString GetKeyCode() const;
  void SetRandomKeyCode();

 private:
  static std::mt19937 gen_;

  QString name_;
  // If owner's ClientId is kNullClientId than owner of this room is server
  ClientId owners_client_id_{};
  int max_clients_{};
  bool is_public_{true};
  QString key_code_;
};

#endif  // SERVER_ROOM_ROOM_SETTINGS_H_
