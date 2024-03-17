#pragma once

#include <cstddef>
#include <functional>
#include <ws2def.h>
#include "Utility/NetUtility.hpp"

namespace std
{
template <typename T>
void hash_combine(std::size_t& InSeed, const T& InValue);

template<> struct hash<sockaddr_in>
{
  std::size_t operator()(const sockaddr_in& InAddress) const
  {
    std::size_t hashResult = 0;
    hash_combine(hashResult, InAddress.sin_addr.s_addr);
    hash_combine(hashResult, InAddress.sin_port);
    return hashResult;
  }
};

// Based on boost::hash_combine
template <typename T>
void hash_combine(std::size_t& InSeed, const T& InValue)
{
  InSeed ^= std::hash<T>{}(InValue) + 0x9e3779b9 + (InSeed << 6) + (InSeed >> 2);
}

template<> struct equal_to<sockaddr_in>
{
  bool operator()(const sockaddr_in& InLhs, const sockaddr_in& InRhs) const
  {
    return InLhs.sin_addr.s_addr == InRhs.sin_addr.s_addr && InLhs.sin_port == InRhs.sin_port;
  }
};

template<> struct hash<sockaddr_storage>
{
  std::size_t operator()(const sockaddr_storage& InAddress) const
  {
    std::size_t hashResult = 0;
    if (InAddress.ss_family == AF_INET)
    {
      const sockaddr_in inAddr = NetUtility::RetrieveIPv4AddressFromStorage(InAddress);
      hash_combine(hashResult, inAddr.sin_addr.s_addr);
      hash_combine(hashResult, inAddr.sin_port);
    }
    return hashResult;
  }
};
}