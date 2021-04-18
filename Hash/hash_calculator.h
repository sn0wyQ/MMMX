//
// Created by matvey on 18.04.21.
//

#ifndef HASH_HASH_CALCULATOR_H_
#define HASH_HASH_CALCULATOR_H_

#include <vector>
#include <QVariant>

namespace HashCalculator {
  QByteArray GetHash(const std::vector<QVariant>& params);
}

#endif  // HASH_HASH_CALCULATOR_H_
