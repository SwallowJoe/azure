#pragma

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <functional>
#include <optional>

#include "VSyncTracker.h"

namespace vsync {
class VSyncDispatcher final {
 public:
  using CallbackToken = int64_t;
  using Callback = std::function<void(nsecs_t wakeupTime)>;

  static std::shared_ptr<VSyncDispatcher> createVSyncDispatcher();

  explicit VSyncDispatcher();
  ~VSyncDispatcher();

  CallbackToken registerCallback(Callback);
  void unregisterCallback(CallbackToken);
  nsecs_t now() const;

 private:
  void threadMain(std::unique_lock<std::mutex>& lock);
  void dispatchLocked();
  void endDispatch();
  void alarmAt(Callback, nsecs_t uptime);
  void alarmCancel();

  mutable std::mutex mMutex;
  mutable std::condition_variable mCondition;
  std::thread mThread;
  std::unordered_map<int64_t, Callback> mCallbacks;
  bool mDestroy = false;
  std::unique_ptr<VSyncTracker> mTracker;
  std::optional<nsecs_t> mLastDispatchTimestamp = std::nullopt;
};
}  // namespace vsync