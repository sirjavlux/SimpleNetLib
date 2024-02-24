#pragma once

#include <vector>
#include <cstdint>

#define SEQUENCE_BITMAP_START_SIZE (64 * 64)
#define BITSIZE_PER_STORAGE_UNIT 64

class SequenceNumberBitmap
{
public:
  SequenceNumberBitmap() : maxSequenceNumber_(SEQUENCE_BITMAP_START_SIZE)
  {
    // Calculate the size of the bitmap vector needed
    const uint64_t size = maxSequenceNumber_ / BITSIZE_PER_STORAGE_UNIT + 1;
    // Initialize bitmap with all zeros
    bitmap_ = std::vector<uint64_t>(size, 0);
  }

  void MarkReceived(const uint64_t InSequenceNumber)
  {
    // If exceeds max number, grow vector
    if (InSequenceNumber > maxSequenceNumber_)
    {
      Grow(InSequenceNumber);
    }
    
    const uint64_t index = InSequenceNumber / BITSIZE_PER_STORAGE_UNIT;
    const uint64_t bitPosition = InSequenceNumber % BITSIZE_PER_STORAGE_UNIT;
    bitmap_[index] |= 1ull << bitPosition;
  }

  bool IsReceived(const uint64_t InSequenceNumber) const
  {
    // If exceeds max number, grow vector
    if (InSequenceNumber > maxSequenceNumber_)
    {
      return false;
    }
    
    const uint64_t index = InSequenceNumber / BITSIZE_PER_STORAGE_UNIT;
    const uint64_t bitPosition = InSequenceNumber % BITSIZE_PER_STORAGE_UNIT;
    return (bitmap_[index] & 1ull << bitPosition) != 0;
  }

private:
  void Grow(const uint64_t InNumber)
  {
    // Calculate Grow Size
      const int index = InNumber / BITSIZE_PER_STORAGE_UNIT;
      const int indexesNeeded = floor(log2f(index));
      const int sizeToGrowTo = powf(2, indexesNeeded + 1);
    
      maxSequenceNumber_ = sizeToGrowTo;
      bitmap_.resize(maxSequenceNumber_);
  }
  
  uint64_t maxSequenceNumber_;
  std::vector<uint64_t> bitmap_;
};