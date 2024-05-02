#pragma once

#include <vector>
#include <cstdint>

#define NUMBER_BITMAP_START_SIZE (64l * 64l)
#define NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT 64l

class NumberBitmap
{
public:
    NumberBitmap() : maxNumber_(NUMBER_BITMAP_START_SIZE)
    {
        // Calculate the size of the bitmap vector needed
        const uint64_t size = maxNumber_ / NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT + 1;
        // Initialize bitmap with all zeros
        bitmap_ = std::vector<uint64_t>(size, 0);
    }

    void MarkSet(const uint64_t InNumber, const bool InShouldBeSet = true)
    {
        // If exceeds max number, grow vector
        while (InNumber >= maxNumber_)
        {
            Grow(InNumber);
        }

        const uint64_t index = InNumber / NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT;
        const uint64_t bitPosition = InNumber % NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT;
        if (InShouldBeSet)
        {
            bitmap_[index] |= 1ull << bitPosition;
        }
        // TODO: Implement this case
        else
        {

        }
    }

    bool IsSet(const uint64_t InNumber) const
    {
        // If exceeds max number, grow vector
        if (InNumber >= maxNumber_)
        {
            return false;
        }

        const uint64_t index = InNumber / NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT;
        const uint64_t bitPosition = InNumber % NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT;
        return (bitmap_[index] & 1ull << bitPosition) != 0;
    }

private:
    void Grow(const uint64_t InNumber)
    {
        maxNumber_ *= 2;
        bitmap_.resize(maxNumber_ / NUMBER_BITMAP_BITSIZE_PER_STORAGE_UNIT + 1);
    }

    uint64_t maxNumber_;
    std::vector<uint64_t> bitmap_;
};