#ifndef GUI_GAMEVIEW_GAME_VIEW_H_
#define GUI_GAMEVIEW_GAME_VIEW_H_

#include <memory>
#include <utility>
#include <vector>

#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Converter/converter.h"
#include "Painter/painter.h"

class ViewPort : public QWidget {
  Q_OBJECT

 public:
  ViewPort(QWidget* parent, std::shared_ptr<ClientGameModel> model);

  std::shared_ptr<Converter> GetConverter();

  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;
  std::shared_ptr<Converter> converter_;
};

#endif  // GUI_GAMEVIEW_GAME_VIEW_H_