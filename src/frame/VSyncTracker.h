#pragma once

#include <mutex>
#include <vector>
#include <unordered_map>
#include <optional>
#include "Time.h"

namespace vsync {
class VSyncTracker {
 private:
  static const int kHistorySize = 30;
  static const int kMinimumSamplesForPrediction = 10;
  std::vector<nsecs_t> mTimestamps;
  int mLastTimestampIndex = 0;
  std::optional<nsecs_t> mKnownTimestamp;
  nsecs_t mPeriod;
  mutable std::mutex mMutex;
  struct Model {
    nsecs_t slope;
    nsecs_t intercept;
  };
  std::unordered_map<nsecs_t, Model> mutable mRateMap;

  Model getVSyncPredictionModelLocked() const;
  void clearTimestampsLocked();
 public:
  nsecs_t nextAnticipatedVSyncTimeFrom(nsecs_t timePoint) const;
  bool addVsyncTimestamp(nsecs_t timestamp);
  nsecs_t getPeriod() const;
  void setPeriod(nsecs_t period);
  explicit VSyncTracker(nsecs_t period);
  ~VSyncTracker();
};

}  // namespace vsync