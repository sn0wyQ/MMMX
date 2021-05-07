#ifndef CLIENT_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_ABSTRACT_CLIENT_VIEW_H_

#include <memory>

#include <QMainWindow>

#include "Converter/converter.h"

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
  virtual std::shared_ptr<Converter> GetConverter() = 0;
  virtual QPointF GetPlayerToCenterOffset() const = 0;
};

#endif  // CLIENT_ABSTRACT_CLIENT_VIEW_H_
