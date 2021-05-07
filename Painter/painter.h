#ifndef PAINTER_PAINTER_H_
#define PAINTER_PAINTER_H_

#include <memory>
#include <utility>

#include <QPainter>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QRegion>
#include <QSize>

#include "Animation/shared_frame.h"
#include "Converter/converter.h"

namespace Constants::Painter {

const QFont kDefaultFont("Roboto Mono", -1, QFont::Weight::Normal);
const QColor kLevelColor(238, 230, 23);

}  // namespace Constants::Painter

namespace PainterEnumsWrapper {

Q_NAMESPACE

enum class DrawPixmapType {
  kUsePointAsCenter,
  kUsePointAsTopLeftCorner
};

Q_ENUM_NS(DrawPixmapType)

}  // namespace PainterEnumsWrapper

using DrawPixmapType = PainterEnumsWrapper::DrawPixmapType;
Q_DECLARE_METATYPE(DrawPixmapType)

class Painter : public QPainter {
 public:
  Painter(QPixmap* device,
          std::shared_ptr<Converter> converter);

  void SetClipCircle(float x, float y, float r,
                     Qt::ClipOperation clip_operation = Qt::ReplaceClip);
  void ResetClip();

  void RotateClockWise(float degree);
  void RotateCounterClockWise(float degree);

  void Translate(const QPointF& delta);

  void DrawEllipse(const QPointF& center, float rx, float ry);
  void DrawPixmap(QPointF point, float w, float h, const QPixmap& pixmap,
                  DrawPixmapType draw_image_type =
                      DrawPixmapType::kUsePointAsCenter);
  void DrawRect(float x, float y, float width, float height);
  void DrawSharedFrame(QPointF point, float w, float h,
                       SharedFrame* shared_frame,
                       DrawPixmapType draw_pixmap_type =
                           DrawPixmapType::kUsePointAsCenter);
  void DrawTriangle(const QPointF& p1, const QPointF& p2, const QPointF& p3);

  void RenderSvg(QPointF point, float w, float h,
                 const std::shared_ptr<QSvgRenderer>& svg_renderer,
                 DrawPixmapType draw_pixmap_type =
                     DrawPixmapType::kUsePointAsCenter);

 private:
  std::shared_ptr<Converter> converter_;
};

#endif  // PAINTER_PAINTER_H_
