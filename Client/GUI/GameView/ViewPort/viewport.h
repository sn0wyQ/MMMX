#ifndef GUI_GAMEVIEW_GAME_VIEW_H_
#define GUI_GAMEVIEW_GAME_VIEW_H_

#include <memory>
#include <utility>
#include <vector>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QOpenGLWidget>
#include <QOpenGLPaintDevice>

#include "Client/GUI/Animations/spring_emulator.h"
#include "Client/GUI/GameView/KillFeed/kill_feed.h"
#include "Client/client_controller.h"
#include "Converter/converter.h"
#include "Painter/painter.h"

class ViewPort : public QOpenGLWidget {
  Q_OBJECT

 public:
  ViewPort(QWidget* parent,
           std::shared_ptr<ClientController> controller);

  std::shared_ptr<Converter> GetConverter();

  void Update();

  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* paint_event) override;
  // We use resizeGL instead of resizeEvent because
  // https://doc.qt.io/qt-5/qopenglwidget.html#resizeEvent
  void resizeGL(int, int) override;

  QPointF GetPlayerToCenterOffset() const;

 private:
  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<ClientGameModel> model_;
  std::shared_ptr<Converter> converter_;

  void DrawObjects(
      const std::vector<std::shared_ptr<GameObject>>& objects,
      const QRectF& view_rect,
      std::vector<std::shared_ptr<GameObject>>* drawn_objects,
      Painter* painter);
  void DrawBars(const std::shared_ptr<GameObject>& object, Painter* painter);

  bool need_to_update_{false};
  bool was_player_set_{false};
  SpringEmulator<QVector2D, false> camera_motion_emulator_;
  SpringEmulator<float, false> fov_change_emulator_;
};

#endif  // GUI_GAMEVIEW_GAME_VIEW_H_
