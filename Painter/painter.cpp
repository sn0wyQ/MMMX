#include "painter.h"

Painter::Painter(QPaintDevice* device,
                 std::shared_ptr<Converter> converter,
                 const QPointF& local_centre)
    : QPainter(device),
      converter_(std::move(converter)) {
  // Setting screen centre to Player's position
  translate((QPointF(device->width(), device->height()) / 2.f)
                - converter_->ScaleFromGameToScreen(local_centre));
}

void Painter::SetClipCircle(float x,
                            float y,
                            float r,
                            Qt::ClipOperation clip_operation) {
  QPointF clip_centre =
      converter_->ScaleFromGameToScreen(QPointF(x - r, y - r));
  int doubled_r_scaled =
      static_cast<int>(converter_->ScaleFromGameToScreen(r * 2));

  QRegion region(static_cast<int>(clip_centre.x()),
                 static_cast<int>(clip_centre.y()),
                 doubled_r_scaled,
                 doubled_r_scaled,
                 QRegion::Ellipse);
  setClipRegion(region, clip_operation);
}

void Painter::RotateClockWise(float degree) {
  rotate(degree);
}

void Painter::RotateCounterClockWise(float degree) {
  rotate(-degree);
}

void Painter::Translate(const QPointF& delta) {
  translate(converter_->ScaleFromGameToScreen(delta));
}

void Painter::DrawEllipse(const QPointF& center, float rx, float ry) {
  drawEllipse(converter_->ScaleFromGameToScreen(center),
              converter_->ScaleFromGameToScreen(rx),
              converter_->ScaleFromGameToScreen(ry));
}

void Painter::DrawTriangle(const QPointF& p1,
                           const QPointF& p2,
                           const QPointF& p3) {
  QPolygonF polygon;
  polygon << converter_->ScaleFromGameToScreen(p1)
          << converter_->ScaleFromGameToScreen(p2)
          << converter_->ScaleFromGameToScreen(p3);
  drawPolygon(polygon);
}
