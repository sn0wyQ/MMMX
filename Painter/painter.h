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
#include <QtSvg/QtSvg>

#include "Converter/converter.h"

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
  Painter(QPaintDevice* device,
          std::shared_ptr<Converter> converter,
          const QPointF& local_centre);

  void SetClipCircle(float x,
                     float y,
                     float r,
                     Qt::ClipOperation clip_operation = Qt::ReplaceClip);

  void RotateClockWise(float degree);
  void RotateCounterClockWise(float degree);

  void Translate(const QPointF& delta);

  void DrawEllipse(const QPointF& center, float rx, float ry);
  void DrawImage(const QString& file_path,
                 QPointF point,
                 float w,
                 float h,
                 DrawPixmapType draw_image_type =
                     DrawPixmapType::kUsePointAsCenter);
  void DrawPixmap(QPointF point,
                  float w,
                  float h,
                  const QPixmap& pixmap,
                  DrawPixmapType draw_image_type =
                      DrawPixmapType::kUsePointAsCenter);
  void DrawRect(float x, float y, float width, float height);
  void DrawTriangle(const QPointF& p1, const QPointF& p2, const QPointF& p3);

 private:
  std::shared_ptr<Converter> converter_;
};

#endif  // PAINTER_PAINTER_H_
