#ifndef CLIENT_ABSTRACT_CLIENT_VIEW_H_
#define CLIENT_ABSTRACT_CLIENT_VIEW_H_

#include <QMainWindow>

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  AbstractClientView() = default;
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
};

#endif  // CLIENT_ABSTRACT_CLIENT_VIEW_H_
