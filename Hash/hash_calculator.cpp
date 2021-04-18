//
// Created by matvey on 18.04.21.
//

#include <QCryptographicHash>

#include "constants.h"
#include "hash_calculator.h"

QByteArray HashCalculator::GetHash(const std::vector<QVariant>& params) {
  QCryptographicHash hash_generator(Constants::kHashAlgorithm);
  for (auto& param : params) {
    hash_generator.addData(param.toByteArray());
  }
  return hash_generator.result();
}
