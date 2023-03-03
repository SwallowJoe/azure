#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "Time.h"
#include "AutoLock.h"
#include "VSyncDispatcher.h"

enum CallbackType {
    CALLBACK_INPUT = 0,
    CALLBACK_ANIMATION = 1,
    CALLBACK_COMMIT = 2
};

class Choreographer final : public vsync::VSyncDispatcher::Callback {
public:
    using FrameCallback = std::function<void(int frameId, nsecs_t wakeupTime)>;

    Choreographer();
    ~Choreographer() = default;
    static std::shared_ptr<Choreographer> getInstance();

    void postFrameCallback(const FrameCallback& callback);
    void postFrameCallbackDelayed(const FrameCallback& callback, long delayMills);
    void removeFrameCallback(const FrameCallback& callback);

    void postCallback(CallbackType type, const FrameCallback& callback);
    void postCallbackDelayed(CallbackType type, const FrameCallback& callback, long delayMills);

    void removeCallback(CallbackType type, const FrameCallback& callback);

    void release() {
        mVSyncDispatcher->endDispatch();
    }
private:

    class FrameCallbackRecord {
        public:
            FrameCallbackRecord(const FrameCallback& callback)
                    : wakeupTimestamp(0), callback(callback) {}

            FrameCallbackRecord(const FrameCallback& callback, nsecs_t wakeupTimestamp)
                    : wakeupTimestamp(wakeupTimestamp), callback(callback) {}

            nsecs_t wakeupTimestamp;
            const FrameCallback& callback;
    };

    void onVsync(nsecs_t wakeupTime);
    void doCallbacks(int callbackType, int frameId, nsecs_t timestamp);

    mutable std::mutex mMutex;
    std::unordered_map<int, std::unordered_set<std::shared_ptr<FrameCallbackRecord>>> mCallbacks;
    std::shared_ptr<vsync::VSyncDispatcher> mVSyncDispatcher;
    uint64_t frameId = 0;
};
