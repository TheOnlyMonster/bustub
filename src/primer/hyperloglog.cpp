#include "primer/hyperloglog.h"

namespace bustub {

template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) : cardinality_(0) {}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  std::bitset<BITSET_CAPACITY> binary_ele(hash);
  return binary_ele;
}

template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  uint64_t pos = 0;
  for (int i = BITSET_CAPACITY - 1; i >= 0; i--) {
    if (bset[i] == 1) {
      pos = i;
      break;
    }
  }
  return pos;
}

template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  hash_t hash = CalculateHash(val);
  std::bitset<BITSET_CAPACITY> binary = ComputeBinary(hash);
  uint64_t pos = PositionOfLeftmostOne(binary);
  uint64_t leading_zeros = 0;
  for (int i = pos; i >= 0; i--) {
    if (binary[i] == 0) {
      leading_zeros++;
    } else {
      break;
    }
  }
  leading_zeros_[pos] = std::max(leading_zeros_[pos], leading_zeros);
}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  double sum = 0;
  for (int i = 0; i < BITSET_CAPACITY; i++) {
    sum += 1.0 / (1 << leading_zeros_[i]);
  }
  double estimate = 0.709 * BITSET_CAPACITY * BITSET_CAPACITY / sum;
  if (estimate <= 2.5 * BITSET_CAPACITY) {
    int zeros = 0;
    for (int i = 0; i < BITSET_CAPACITY; i++) {
      if (leading_zeros_[i] == 0) {
        zeros++;
      }
    }
    if (zeros != 0) {
      estimate = BITSET_CAPACITY * std::log(BITSET_CAPACITY / zeros);
    }
  } else if (estimate > (1.0 / 30.0) * std::pow(2, 32)) {
    estimate = -std::pow(2, 32) * std::log(1 - estimate / std::pow(2, 32));
  }
  cardinality_ = static_cast<size_t>(estimate);
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
