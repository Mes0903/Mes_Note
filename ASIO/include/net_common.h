#ifndef NET_COMMON
#define NET_COMMON

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#ifdef _WIN32
#define _WIN32_WINNT 0X0A00
#endif

#define ASIO_STANDALONE
#include <boost/asio.hpp>

#endif