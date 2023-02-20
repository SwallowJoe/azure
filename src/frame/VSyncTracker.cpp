#include <algorithm>

#include "Log.h"
#include "VSyncTracker.h"

//#define DEBUG_TRACKER

namespace vsync {

VSyncTracker::VSyncTracker(nsecs_t period)
    : mLastTimestampIndex(0), mPeriod(period) {
  mRateMap[mPeriod] = {mPeriod, 0};
}

VSyncTracker::~VSyncTracker() {}

VSyncTracker::Model VSyncTracker::getVSyncPredictionModelLocked() const {
  return mRateMap.find(mPeriod)->second;
}

void VSyncTracker::clearTimestampsLocked() {
  if (!mTimestamps.empty()) {
    LOGE("clearTimestampsLocked");
    auto const maxRb =
        *std::max_element(mTimestamps.begin(), mTimestamps.end());
    if (mKnownTimestamp) {
      mKnownTimestamp = std::max(*mKnownTimestamp, maxRb);
    } else {
      mKnownTimestamp = maxRb;
    }

    mTimestamps.clear();
    mLastTimestampIndex = 0;
  }
}

nsecs_t VSyncTracker::nextAnticipatedVSyncTimeFrom(nsecs_t timePoint) const {
  std::lock_guard lock(mMutex);
  auto const [slope, intercept] = getVSyncPredictionModelLocked();
  if (mTimestamps.empty()) {
    auto const knownTimestamp = mKnownTimestamp ? *mKnownTimestamp : timePoint;
    auto const numPeriodsOut = ((timePoint - timePoint) / mPeriod) + 1;
    return timePoint + numPeriodsOut * mPeriod;
  }

  auto const oldest = *std::min_element(mTimestamps.begin(), mTimestamps.end());

  auto const zeroPoint = oldest + intercept;
  auto const ordinalRequest = (timePoint - zeroPoint + slope) / slope;
  auto const prediction = (ordinalRequest * slope) + intercept + oldest;

  return prediction;
}

bool VSyncTracker::addVsyncTimestamp(nsecs_t timestamp) {
  std::lock_guard lock(mMutex);

  const size_t numSamples = mTimestamps.size();
  if (numSamples < kMinimumSamplesForPrediction) {
    mTimestamps.push_back(timestamp);
    mKnownTimestamp = std::max(
        timestamp, *std::max_element(mTimestamps.begin(), mTimestamps.end()));
    return false;
  }
  if (numSamples != kHistorySize) {
    mTimestamps.push_back(timestamp);
    mLastTimestampIndex = (mLastTimestampIndex + 1) % numSamples;
  } else {
    mLastTimestampIndex = (mLastTimestampIndex + 1) % numSamples;
    mTimestamps[mLastTimestampIndex] = timestamp;
  }

  std::vector<nsecs_t> vsyncTS(numSamples);
  std::vector<nsecs_t> ordinals(numSamples);

  const auto oldestTS =
      *std::min_element(mTimestamps.begin(), mTimestamps.end());
  auto it = mRateMap.find(mPeriod);
  auto const currentPeriod = it->second.slope;

  constexpr int64_t kScalingFactor = 1000;
  nsecs_t meanTS = 0;
  nsecs_t meanOrdinal = 0;

  for (size_t i = 0; i < numSamples; i++) {
    const auto timestamp = mTimestamps[i] - oldestTS;
    vsyncTS[i] = timestamp;
    meanTS += timestamp;

    const auto ordinal =
        (vsyncTS[i] + currentPeriod / 2) / currentPeriod * kScalingFactor;
    ordinals[i] = ordinal;
    meanOrdinal += ordinal;
  }

  meanTS /= numSamples;
  meanOrdinal /= numSamples;

  for (size_t i = 0; i < numSamples; i++) {
    vsyncTS[i] -= meanTS;
    ordinals[i] -= meanOrdinal;
  }

  nsecs_t top = 0;
  nsecs_t bottom = 0;
  for (size_t i = 0; i < numSamples; i++) {
    top += vsyncTS[i] * ordinals[i];
    bottom += ordinals[i] * ordinals[i];
  }

  if (bottom == 0) {
    // divid zero?
    clearTimestampsLocked();
    return false;
  }

  nsecs_t const anticipatedPeriod = top * kScalingFactor / bottom;
  nsecs_t const intercept =
      meanTS - (anticipatedPeriod * meanOrdinal / kScalingFactor);

  static auto constexpr kMaxPercent = 100u;
#ifdef __linux__
  constexpr uint32_t kDiscardOutlierPercent = 20;
#else
  // for windows is not a real-time os.
  constexpr uint32_t kDiscardOutlierPercent = 50;
#endif
  auto const percent =
      std::abs(anticipatedPeriod - mPeriod) * kMaxPercent / mPeriod;
  if (percent >= kDiscardOutlierPercent) {
    // 误差太大
    clearTimestampsLocked();
    return false;
  }

#ifdef DEBUG_TRACKER_1
  LOGD("VSP-period: ", anticipatedPeriod);
  LOGD("VSP-intercept: ", intercept);
#endif

  static bool sHasUpdate = false;
  if (sHasUpdate) {
    return false;
  }
  sHasUpdate = true;
  it->second = {anticipatedPeriod, intercept};

#ifdef DEBUG_TRACKER
  LOGI("model update ts: ", timestamp, " slope: ", anticipatedPeriod,
       " intercept: ", intercept);
#endif
  return true;
}

nsecs_t VSyncTracker::getPeriod() const {
  std::lock_guard lock(mMutex);
  return mRateMap.find(mPeriod)->second.slope;
}
void VSyncTracker::setPeriod(nsecs_t period) {
  std::lock_guard lock(mMutex);
  mPeriod = period;
  if (mRateMap.find(period) == mRateMap.end()) {
    mRateMap[period] = {mPeriod, 0};
  }
  clearTimestampsLocked();
}
}  // namespace vsync