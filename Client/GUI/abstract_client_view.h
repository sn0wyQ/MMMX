#ifndef CLIENT_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_ABSTRACT_CLIENT_VIEW_H_

#include <memory>

#include <QMainWindow>

#include "Converter/converter.h"

namespace ClientWindowTypeWrapper {

Q_NAMESPACE

enum class ClientWindowType {
  // First enum MUST be 0
  kMainMenu = 0,
  kGameView
};

Q_ENUM_NS(ClientWindowType)

}  // namespace ClientWindowTypeWrapper
using ClientWindowType = ClientWindowTypeWrapper::ClientWindowType;
Q_DECLARE_METATYPE(ClientWindowType)


class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
  virtual std::shared_ptr<Converter> GetConverter() = 0;
  virtual void SetWindow(ClientWindowType window_type) = 0;
};

#endif  // CLIENT_ABSTRACT_CLIENT_VIEW_H_
