#ifndef PAINTER_PAINTER_H_
#define PAINTER_PAINTER_H_

#include <memory>
#include <utility>

#include <QPainter>
#include <QPointF>
#include <QPolygonF>
#include <QRegion>

#include "Converter/converter.h"

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
  void DrawTriangle(const QPointF& p1, const QPointF& p2, const QPointF& p3);
  void DrawRect(float x, float y, float width, float height);
  void DrawText(QRectF rect, int flags, const QString& str);

 private:
  std::shared_ptr<Converter> converter_;
};

#endif  // PAINTER_PAINTER_H_
