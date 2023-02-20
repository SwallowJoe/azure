#pragma once

#include <mutex>
#include <thread>

class Mutex {
 private:
  std::mutex mLock;
  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex) = delete;

 public:
  inline Mutex() {}
  inline void lock() { mLock.lock(); }
  inline void unlock() { mLock.unlock(); }
};

class AutoLock {
 private:
  Mutex& mLock;
  AutoLock(const AutoLock&) = delete;
  AutoLock& operator=(const AutoLock) = delete;

 public:
  inline explicit AutoLock(Mutex& lock) : mLock(lock) { mLock.lock(); }
  inline explicit AutoLock(Mutex* lock) : mLock(*lock) { mLock.lock(); }
  inline ~AutoLock() { mLock.unlock(); }
};