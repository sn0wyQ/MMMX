#ifndef SERVER_ROOM_ROOM_INFO_H_
#define SERVER_ROOM_ROOM_INFO_H_

#include <QList>
#include <QVariant>

struct RoomInfo {
  RoomId id;
  QString name;
  int max_clients;
  ClientId owners_client_id;
  bool is_public;
  QList<ClientId> clients;

  QList<QVariant> GetVariantList() {
    QList<QVariant> variant_list;
    variant_list.push_back(id);
    variant_list.push_back(name);
    variant_list.push_back(max_clients);
    variant_list.push_back(owners_client_id);
    variant_list.push_back(is_public);
    QList<QVariant> clients_list;
    for (ClientId client : clients) {
      clients_list.push_back(client);
    }
    return variant_list;
  }

  static RoomInfo GetRoomInfoFromVariantList(
      const QList<QVariant>& variant_list) {
    RoomInfo room_info;
    room_info.id = variant_list.at(0).toInt();
    room_info.name = variant_list.at(1).toString();
    room_info.max_clients = variant_list.at(2).toInt();
    room_info.owners_client_id = variant_list.at(3).toInt();
    room_info.is_public = variant_list.at(4).toBool();
    QList<QVariant> clients;
    for (const auto& variant : clients) {
      room_info.clients.push_back(variant.toInt());
    }
    return room_info;
  }
};

#endif  // SERVER_ROOM_ROOM_INFO_H_
