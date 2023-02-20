
#include <chrono>
#include <iostream>
#include <mutex>

#include "Looper.h"
#include "Log.h"

//#define DEBUG_LOOPER 1
#define DEBUG_LOOPER_DELAY 1

using namespace std::chrono_literals;

Looper::Looper() : mThreadName("Looper") {
  mThread = std::thread([this]() {
    std::unique_lock<std::mutex> lock(mMutex);
    threadMain(lock);
  });
  pthread_setname_np(mThread.native_handle(), mThreadName);
}

Looper::~Looper() {
#ifdef DEBUG_LOOPER
  LOGD("~Looper()");
#endif
  // mThreadRunning = false;
  mThread.join();
}

void Looper::sendMessage(const std::shared_ptr<MessageHandler> handler,
                         const Message& message) {
  sendMessageDelay(handler, message, 0);
}

void Looper::sendMessageDelay(const std::shared_ptr<MessageHandler> handler,
                              const Message& message, int millisecond) {
  MessageEntry entry;
  entry.handler = handler;
  entry.message = message;
  entry.uptime = systemTime(SYSTEM_TIME_MONOTONIC) + ms2ns(millisecond);

  std::lock_guard lock(mMutex);
  auto it = mMessageEntries.begin();
  while (it != mMessageEntries.end()) {
    if ((*it).uptime > entry.uptime) {
      break;
    }
    it++;
  }
  mMessageEntries.insert(it, entry);
  mCondition.notify_one();
}

bool Looper::threadMain(std::unique_lock<std::mutex>& lock) {
#ifdef DEBUG_LOOPER
  LOGD(mThreadName, " start.");
#endif
  do {
#ifdef DEBUG_LOOPER
    LOGD("Message size=", mMessageEntries.size());
#endif
    auto now = systemTime(SYSTEM_TIME_MONOTONIC);
    auto waitTime = std::numeric_limits<int64_t>::max();
    auto it = mMessageEntries.begin();
    while (it != mMessageEntries.end()) {
      if ((*it).uptime > now) {
        waitTime = (*it).uptime - now;
        break;
      }
#ifdef DEBUG_LOOPER_DELAY
      auto dispatchDelay =
          ns2ms(systemTime(SYSTEM_TIME_MONOTONIC) - (*it).uptime);
      if (dispatchDelay > 0) {
        LOGD("dispatchDelay(",(*it).message.what,")=", dispatchDelay,"ms");
      }

#endif
      (*it).handler->handleMessage((*it).message);
      it = mMessageEntries.erase(it);
    }
#ifdef DEBUG_LOOPER
    LOGD("Message size=" , mMessageEntries.size(), " but need wait=" , waitTime);
#endif
    mCondition.wait_for(lock,
                        std::chrono::duration<nsecs_t, std::nano>(waitTime));
  } while (mThreadRunning);
#ifdef DEBUG_LOOPER
  LOGI(mThreadName, " stop.");
#endif
  return true;
}

MessageHandler::~MessageHandler() {}