#ifndef ABSTRACT_CLIENT_VIEW_H
#define ABSTRACT_CLIENT_VIEW_H

#include <QMainWindow>

class AbstractClientView : public QMainWindow {
  Q_OBJECT

 public:
  AbstractClientView() = default;
  ~AbstractClientView() override = default;

  virtual void Update() = 0;
};

#endif //ABSTRACT_CLIENT_VIEW_H
