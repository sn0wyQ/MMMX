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
#include "SpringEmulator/spring_emulator.h"
#include "Painter/painter.h"

class GameView : public QWidget {
  Q_OBJECT

 public:
  GameView(QWidget* parent, std::shared_ptr<ClientGameModel> model);

  std::shared_ptr<Converter> GetConverter();

  void Update();
  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

  QPointF GetPlayerToCenterOffset() const;

 private:
  std::shared_ptr<ClientGameModel> model_;
  std::shared_ptr<Converter> converter_;

  bool was_player_set_;
  SpringEmulator<QVector2D> camera_motion_emulator_;
  SpringEmulator<float> fov_change_emulator_;
  std::unique_ptr<QPixmap> canvas_;
  std::unique_ptr<Painter> painter_;
};

#endif  // GUI_GAMEVIEW_GAME_VIEW_H_
