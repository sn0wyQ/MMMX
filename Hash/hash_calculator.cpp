#include <QCryptographicHash>

#include "Constants/constants.h"
#include "hash_calculator.h"

QByteArray HashCalculator::GetHash(const std::vector<QVariant>& params) {
  QCryptographicHash hash_generator(Constants::kHashAlgorithm);
  for (const auto& param : params) {
    hash_generator.addData(param.toByteArray());
  }
  return hash_generator.result();
}
