#include "painter.h"

Painter::Painter(QPaintDevice* device,
                 std::shared_ptr<Converter> converter,
                 const QPointF& local_centre)
    : QPainter(device),
      converter_(std::move(converter)) {
  // Setting screen centre to Player's position
  translate((QPointF(device->width(), device->height()) / 2.f)
                - converter_->FromGameToScreen(local_centre));
}

void Painter::RotateClockWise(float degree) {
  rotate(degree);
}

void Painter::RotateCounterClockWise(float degree) {
  rotate(-degree);
}

void Painter::Translate(const QPointF& delta) {
  translate(converter_->FromGameToScreen(delta));
}

void Painter::DrawEllipse(const QPointF& center, float rx, float ry) {
  drawEllipse(converter_->FromGameToScreen(center),
              converter_->FromGameToScreen(rx),
              converter_->FromGameToScreen(ry));
}

void Painter::DrawTriangle(const QPointF& p1,
                           const QPointF& p2,
                           const QPointF& p3) {
  QPolygonF polygon;
  polygon << converter_->FromGameToScreen(p1)
          << converter_->FromGameToScreen(p2)
          << converter_->FromGameToScreen(p3);
  drawPolygon(polygon);
}
