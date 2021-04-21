#ifndef PAINTER_PAINTER_H_
#define PAINTER_PAINTER_H_

#include <memory>
#include <utility>

#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QRegion>
#include <QSize>

#include "Converter/converter.h"

namespace PainterEnumsWrapper {

Q_NAMESPACE

enum class DrawImageType {
  kUsePointAsCenter,
  kUsePointAsTopLeftCorner
};

Q_ENUM_NS(DrawImageType)

}  // namespace PainterEnumsWrapper

using DrawImageType = PainterEnumsWrapper::DrawImageType;
Q_DECLARE_METATYPE(DrawImageType)

class Painter : public QPainter {
 public:
  Painter(QPaintDevice* device,
          std::shared_ptr<Converter> converter,
          const QPointF& local_centre);

  void SetClipCircle(float x,
                     float y,
                     float r,
                     Qt::ClipOperation clip_operation = Qt::IntersectClip);

  void RotateClockWise(float degree);
  void RotateCounterClockWise(float degree);

  void Translate(const QPointF& delta);

  void DrawEllipse(const QPointF& center, float rx, float ry);
  void DrawImage(const QString& file_path,
                 QPointF point,
                 float w,
                 float h,
                 DrawImageType draw_image_type =
                     DrawImageType::kUsePointAsCenter);
  void DrawTriangle(const QPointF& p1, const QPointF& p2, const QPointF& p3);
  void DrawRect(float x, float y, float width, float height);

 private:
  std::shared_ptr<Converter> converter_;
};

#endif  // PAINTER_PAINTER_H_
