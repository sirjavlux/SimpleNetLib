#pragma once

#include <ws2def.h>
#include <cstddef>
#include <functional>

namespace std
{
template<> struct hash<sockaddr_in>
{
  std::size_t operator()(const sockaddr_in& InAddress) const
  {
    std::size_t hash = 0;
    hash_combine(hash, InAddress.sin_addr.s_addr);
    hash_combine(hash, InAddress.sin_port);
    return hash;
  }
  
  // Based on boost::hash_combine
  template <typename T>
  void hash_combine(std::size_t& InSeed, const T& InValue) const
  {
    InSeed ^= std::hash<T>{}(InValue) + 0x9e3779b9 + (InSeed << 6) + (InSeed >> 2);
  }
};

template<> struct equal_to<sockaddr_in>
{
  bool operator()(const sockaddr_in& InLhs, const sockaddr_in& InRhs) const
  {
    return InLhs.sin_addr.s_addr == InRhs.sin_addr.s_addr && InLhs.sin_port == InRhs.sin_port;
  }
};
}