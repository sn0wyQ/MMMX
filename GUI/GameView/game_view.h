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

class GameView : public QWidget {
  Q_OBJECT

 public:
  GameView(QWidget* parent, std::shared_ptr<GameDataModel> model);

  std::shared_ptr<Converter> GetConverter();

  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

 private:
  std::shared_ptr<GameDataModel> model_;
  std::shared_ptr<Converter> converter_;
};

#endif  // GUI_GAMEVIEW_GAME_VIEW_H_
