#include "VSyncDispatcher.h"

#include <chrono>

#include "Log.h"
#include "Time.h"

using namespace std::chrono_literals;
namespace vsync {

std::shared_ptr<VSyncDispatcher> VSyncDispatcher::createVSyncDispatcher() {
  return std::make_shared<VSyncDispatcher>();
}

VSyncDispatcher::VSyncDispatcher() {
  mTracker = std::make_unique<VSyncTracker>(ms2ns(16));
  mThread = std::thread([this]() {
    std::unique_lock lock(mMutex);
    threadMain(lock);
  });
}

VSyncDispatcher::~VSyncDispatcher() {
  mDestroy = false;
  mThread.join();
}

VSyncDispatcher::CallbackToken VSyncDispatcher::registerCallback(
    Callback callback) {
  std::lock_guard lock(mMutex);
  auto token = mCallbacks.size() + 1;
  mCallbacks.emplace(token, callback);
  mCondition.notify_one();
  return token;
}

void VSyncDispatcher::unregisterCallback(VSyncDispatcher::CallbackToken token) {
  std::lock_guard lock(mMutex);
  mCallbacks.erase(token);

  mCondition.notify_one();
}

nsecs_t VSyncDispatcher::now() const {
  return systemTime(SYSTEM_TIME_MONOTONIC);
}

void VSyncDispatcher::threadMain(std::unique_lock<std::mutex>& lock) {
  while (true) {
    auto nowTime = now();
    auto uptime = mCallbacks.empty()
                      ? std::numeric_limits<nsecs_t>::max()
                      : mTracker->nextAnticipatedVSyncTimeFrom(nowTime);
#ifdef DEBUG_DISPATCHER
    LOGD("threadMain wait: ", uptime - nowTime);
#endif
    mCondition.wait_for(
        lock, std::chrono::duration<nsecs_t, std::nano>(uptime - nowTime));
    dispatchLocked();
    if (mDestroy) {
      return;
    }
  }
}
void VSyncDispatcher::dispatchLocked() {
  auto wakeupTimestamp = now();
  if (mLastDispatchTimestamp) {
    if (wakeupTimestamp - (*mLastDispatchTimestamp) <
        mTracker->getPeriod() / 2) {
      // skip for too nearly.
      return;
    }
  }
  mLastDispatchTimestamp = wakeupTimestamp;
  mTracker->addVsyncTimestamp(wakeupTimestamp);
  auto it = mCallbacks.begin();
  while (it != mCallbacks.end()) {
    ((*it).second)(wakeupTimestamp);
    it++;
  }
}
void VSyncDispatcher::endDispatch() { mDestroy = false; }
void VSyncDispatcher::alarmAt(VSyncDispatcher::Callback, nsecs_t uptime) {}
void VSyncDispatcher::alarmCancel() {}

}  // namespace vsync