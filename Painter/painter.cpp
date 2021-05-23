#include "painter.h"

Painter::Painter(QPaintDevice* device,
                 std::shared_ptr<Converter> converter)
    : QPainter(device),
      converter_(std::move(converter)) {}

void Painter::SetClipCircle(float x,
                            float y,
                            float r,
                            Qt::ClipOperation clip_operation) {
  QPointF clip_center =
      converter_->ScaleFromGameToScreen(QPointF(x - r, y - r));
  int doubled_r_scaled =
      static_cast<int>(converter_->ScaleFromGameToScreen(r * 2));

  QPoint clip_center_in_integers = clip_center.toPoint();
  QRegion region(clip_center_in_integers.x(),
                 clip_center_in_integers.y(),
                 doubled_r_scaled,
                 doubled_r_scaled,
                 QRegion::Ellipse);
  this->setClipRegion(region, clip_operation);
}

void Painter::ResetClip() {
  this->setClipRect(QRect(), Qt::NoClip);
}

void Painter::RotateClockWise(float degree) {
  rotate(degree);
}

void Painter::RotateCounterClockWise(float degree) {
  rotate(-degree);
}

void Painter::Translate(const QPointF& delta) {
  translate(converter_->ScaleFromGameToScreen(delta).toPoint());
}

void Painter::DrawEllipse(const QPointF& center, float rx, float ry) {
  drawEllipse(converter_->ScaleFromGameToScreen(center).toPoint(),
              static_cast<int>(converter_->ScaleFromGameToScreen(rx)),
              static_cast<int>(converter_->ScaleFromGameToScreen(ry)));
}

void Painter::DrawRect(float x, float y, float width, float height) {
  drawRect(QRectF(converter_->ScaleFromGameToScreen(x),
                  converter_->ScaleFromGameToScreen(y),
                  converter_->ScaleFromGameToScreen(width),
                  converter_->ScaleFromGameToScreen(height)).toRect());
}

void Painter::DrawSharedFrame(QPointF point,
                              float w,
                              float h,
                              SharedFrame* shared_frame,
                              DrawPixmapType draw_pixmap_type) {
  converter_->ScaleFromGameToScreen(&point);
  converter_->ScaleFromGameToScreen(&w);
  converter_->ScaleFromGameToScreen(&h);

  if (draw_pixmap_type == DrawPixmapType::kUsePointAsCenter) {
    point.rx() -= (w / 2.f);
    point.ry() -= (h / 2.f);
  }

  drawPixmap(point, *shared_frame->GetRenderedPixmap(static_cast<int>(w),
                                                     static_cast<int>(h)));
}

void Painter::DrawSharedMapFrame(QPointF point,
                                 float w,
                                 float h,
                                 SharedFrame* shared_frame,
                                 DrawPixmapType draw_pixmap_type) {
  converter_->ScaleFromGameToScreen(&point);
  converter_->ScaleFromGameToScreen(&w);
  converter_->ScaleFromGameToScreen(&h);

  QPointF end_point(point.x() + w, point.y() + h);
  if (draw_pixmap_type == DrawPixmapType::kUsePointAsCenter) {
    point.rx() -= (w / 2.f);
    end_point.rx() -= (w / 2.f);
    point.ry() -= (h / 2.f);
    end_point.ry() -= (h / 2.f);
  }

  float pixmap_w = w / Constants::kCountOfPixmapsInSide;
  float pixmap_h = h / Constants::kCountOfPixmapsInSide;
  auto pixmap = *shared_frame->GetRenderedPixmap(
      static_cast<int>(std::ceil(pixmap_w)),
      static_cast<int>(std::ceil(pixmap_h)));
  QPointF new_point = point;
  for (int i = 0; i < Constants::kCountOfPixmapsInSide; i++) {
    for (int j = 0; j < Constants::kCountOfPixmapsInSide; j++) {
      drawPixmap(new_point, pixmap);
      new_point.rx() += pixmap_w;
    }
    new_point.rx() = point.x();
    new_point.ry() += pixmap_h;
  }
}

void Painter::DrawTriangle(const QPointF& p1,
                           const QPointF& p2,
                           const QPointF& p3) {
  QPolygon polygon;
  polygon << converter_->ScaleFromGameToScreen(p1).toPoint()
          << converter_->ScaleFromGameToScreen(p2).toPoint()
          << converter_->ScaleFromGameToScreen(p3).toPoint();
  drawPolygon(polygon);
}
