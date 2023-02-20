#pragma once

//#if defined(__linux__)
#include <ctime>
#include <chrono>
//#else
#include <sys/time.h>
#include <sys/timeb.h>
//#endif

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#ifndef LOG_TAG
#define LOG_TAG "NO_TAG"
#endif

#define LOG(...) __println(__VA_ARGS__)
#define LOGI(...) __println_I(__VA_ARGS__)
#define LOGD(...) __println_D(__VA_ARGS__)
#define LOGW(...) __println_W(__VA_ARGS__)
#define LOGE(...) __println_E(__VA_ARGS__)

static char* log_Time(void) {
  struct tm* ptm;
  struct timeb stTimeb;
  static char szTime[19];
  ftime(&stTimeb);
  ptm = localtime(&stTimeb.time);
  sprintf(szTime, "%02d-%02d %02d:%02d:%02d.%03d", ptm->tm_mon + 1,
          ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
          stTimeb.millitm);
  szTime[18] = 0;
  return szTime;
}
// #endif

static void __print(const std::stringstream& sstream) {
  std::cout << sstream.str() << std::endl << std::flush;
}

template <typename T>
static void __print(std::stringstream& sstream, T& value) {
  sstream << value << std::endl;
  std::cout << sstream.str() << std::flush;
}

template <typename T, typename... Args>
static void __print(std::stringstream& sstream, T& value, Args... args) {
  sstream << value;
  __print(sstream, args...);
}

template <typename T, typename... Args>
static void __println(const T& value, Args... args) {
  using namespace std;
  stringstream ss;

  ss << log_Time() << " " << std::this_thread::get_id() << " D/" << LOG_TAG
     << ": " << value;
  __print(ss, args...);
}

template <typename T, typename... Args>
static void __println_I(const T& value, Args... args) {
  using namespace std;
  stringstream ss;
  ss << log_Time() << " " << std::this_thread::get_id() << " I/" << LOG_TAG
     << ": " << value;
  __print(ss, args...);
}
template <typename T, typename... Args>
static void __println_D(const T& value, Args... args) {
  using namespace std;
  stringstream ss;
  ss << log_Time() << " " << std::this_thread::get_id() << " D/" << LOG_TAG
     << ": " << value;
  __print(ss, args...);
}
template <typename T, typename... Args>
static void __println_W(const T& value, Args... args) {
  using namespace std;
  stringstream ss;
  ss << log_Time() << " " << std::this_thread::get_id() << " W/" << LOG_TAG
     << ": " << value;
  __print(ss, args...);
}
template <typename T, typename... Args>
static void __println_E(const T& value, Args... args) {
  using namespace std;
  stringstream ss;
  ss << log_Time() << " " << std::this_thread::get_id() << " E/" << LOG_TAG
     << ": " << value;
  __print(ss, args...);
}

//--------------ONLY FOR SINGLE THREAD!!!-----------------------------
static std::ostream& logd(void) {
  using namespace std;

  return cout << log_Time() << " D/" << LOG_TAG << ": " << std::flush;
}

static std::ostream& logi(void) {
  using namespace std;

  return cout << log_Time() << " I/" << LOG_TAG << ": " << std::flush;
}

static std::ostream& logw(void) {
  using namespace std;

  return cout << log_Time() << " W/" << LOG_TAG << ": " << std::flush;
}

static std::ostream& loge(void) {
  using namespace std;

  return cout << log_Time() << " E/" << LOG_TAG << ": " << std::flush;
}
