#ifndef CLIENT_CLIENT_VIEW_H_
#define CLIENT_CLIENT_VIEW_H_

#include <memory>
#include <utility>

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QRandomGenerator>

#include "Client/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Event/event.h"

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(std::shared_ptr<ClientController> controller,
                      QWidget* parent = nullptr);
  ~ClientView() override = default;

  void Update() override;

  public Q_SLOTS:
  void OnTestButtonPressed();

 private:
  void paintEvent(QPaintEvent* paint_event) override;
  void keyPressEvent(QKeyEvent* key_event) override;

  std::shared_ptr<ClientController> controller_;

  QPushButton* test_button_;
  QLabel* my_counter_label_;
  QLabel* enemy_counter_label_;
};

#endif  // CLIENT_CLIENT_VIEW_H_
