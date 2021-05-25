#ifndef PLAYER_SELECT_H_
#define PLAYER_SELECT_H_

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

#include "GameObject/MovableObject/Entity/Player/player_enums.h"

class PlayerSelect : public QDialog {
 public:
  explicit PlayerSelect(QWidget* parent,
                        Qt::WindowFlags flags = Qt::WindowFlags());

 private:
  QLabel* label_;
  QPushButton* select_smasher_;
  QPushButton* select_soldier_;
  QPushButton* select_spider_;
  QPushButton* select_viking_;
  QPushButton* cancel_;

  QVBoxLayout* layout_;
};

#endif  // PLAYER_SELECT_H_
