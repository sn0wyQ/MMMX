#ifndef CLIENT_KEYCONTROLLER_KEY_NAMES_H_
#define CLIENT_KEYCONTROLLER_KEY_NAMES_H_

#include <unordered_map>
#include <QString>

namespace KeyNames {

extern const std::unordered_map<uint32_t, QString> kNativeCodeToKeyName;

}  // namespace KeyNames

#endif  // CLIENT_KEYCONTROLLER_KEY_NAMES_H_
