#ifndef GUI_GAMELABEL_GAME_LABEL_H_
#define GUI_GAMELABEL_GAME_LABEL_H_

#include <memory>
#include <utility>

#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>

#include "Client/client_controller.h"
#include "Converter/converter.h"
#include "Painter/painter.h"

class GameLabel : public QLabel {
 public:
  GameLabel(QWidget* parent, GameDataModel* model);

  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

 private:
  GameDataModel* model_;
  std::shared_ptr<Converter> converter_;
};

#endif  // GUI_GAMELABEL_GAME_LABEL_H_
