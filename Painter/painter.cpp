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
  QPointF clip_center =
      converter_->ScaleFromGameToScreen(QPointF(x - r, y - r));
  int doubled_r_scaled =
      static_cast<int>(converter_->ScaleFromGameToScreen(r * 2));

  QRegion region(static_cast<int>(clip_center.x()),
                 static_cast<int>(clip_center.y()),
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

void Painter::DrawPixmap(QPointF point,
                         float w,
                         float h,
                         const QPixmap& pixmap,
                         DrawPixmapType draw_pixmap_type) {
  converter_->ScaleFromGameToScreen(&point);
  converter_->ScaleFromGameToScreen(&w);
  converter_->ScaleFromGameToScreen(&h);

  if (draw_pixmap_type == DrawPixmapType::kUsePointAsCenter) {
    point.rx() -= (w / 2.f);
    point.ry() -= (h / 2.f);
  }

  QRect pixmap_frame
      (point.toPoint(), QSize(static_cast<int>(w), static_cast<int>(h)));
  drawPixmap(pixmap_frame, pixmap);
}

void Painter::DrawRect(float x, float y, float width, float height) {
  drawRect(QRectF(converter_->ScaleFromGameToScreen(x),
                  converter_->ScaleFromGameToScreen(y),
                  converter_->ScaleFromGameToScreen(width),
                  converter_->ScaleFromGameToScreen(height)));
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

void Painter::RenderSvg(QPointF point,
                        float w,
                        float h,
                        const std::shared_ptr<QSvgRenderer>& svg_renderer,
                        DrawPixmapType draw_pixmap_type) {
  converter_->ScaleFromGameToScreen(&point);
  converter_->ScaleFromGameToScreen(&w);
  converter_->ScaleFromGameToScreen(&h);

  if (draw_pixmap_type == DrawPixmapType::kUsePointAsCenter) {
    point.rx() -= (w / 2.f);
    point.ry() -= (h / 2.f);
  }

  QRectF pixmap_frame
      (point, QSize(static_cast<int>(w), static_cast<int>(h)));
  svg_renderer->render(this, pixmap_frame);
}
