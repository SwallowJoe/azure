#include "Choreographer.h"
#include "Log.h"

static std::shared_ptr<Choreographer> sInstance = nullptr;

Choreographer::Choreographer() {
    mVSyncDispatcher = vsync::VSyncDispatcher::createVSyncDispatcher();
    mVSyncDispatcher->registerCallback(std::bind(&Choreographer::onVsync, this, std::placeholders::_1));

    mCallbacks.emplace(CALLBACK_INPUT, std::unordered_set<std::shared_ptr<FrameCallbackRecord>>());
    mCallbacks.emplace(CALLBACK_ANIMATION, std::unordered_set<std::shared_ptr<FrameCallbackRecord>>());
    mCallbacks.emplace(CALLBACK_COMMIT, std::unordered_set<std::shared_ptr<FrameCallbackRecord>>());
}

std::shared_ptr<Choreographer> Choreographer::getInstance() {
    if (sInstance == nullptr) {
        sInstance = std::make_shared<Choreographer>();
    }
    return sInstance;
}

void Choreographer::postFrameCallback(const FrameCallback& callback) {
    // LOGI("postFrameCallback");
    postFrameCallbackDelayed(callback, 0);
}

void Choreographer::postFrameCallbackDelayed(const FrameCallback& callback, long delayMills) {
    postCallbackDelayed(CALLBACK_ANIMATION, callback, delayMills);
}

void Choreographer::removeFrameCallback(const FrameCallback& callback) {
    removeCallback(CALLBACK_ANIMATION, callback);
}

void Choreographer::postCallback(CallbackType type, const FrameCallback& callback) {
    postCallbackDelayed(type, callback, 0);
}

void Choreographer::postCallbackDelayed(CallbackType type, const FrameCallback& callback, long delayMills) {
    if (callback == nullptr) {
        return;
    }
    if (delayMills < 0) {
        return;
    }
    if (type < CALLBACK_INPUT || type > CALLBACK_COMMIT) {
        return;
    }
    AutoLock lock(mMutex);
    mCallbacks.at(type).insert(std::make_shared<FrameCallbackRecord>(callback, now() + ms2ns(delayMills)));
}

void Choreographer::removeCallback(CallbackType type, const FrameCallback& callback) {
    if (callback == nullptr) {
        return;
    }
    /*
    AutoLock lock(mMutex);    
    for (auto it = mCallbacks.at(type).begin();it != mCallbacks.at(type).end(); it++) {
        if ((*it).get()->callback == callback) {
            mCallbacks.at(type).erase(*it);
        }
    }
    */
}

void Choreographer::onVsync(nsecs_t wakeupTime) {
    AutoLock lock(mMutex);
    frameId++;
    for (int type = CALLBACK_INPUT; type <= CALLBACK_COMMIT; type++) {
        auto typeCallbacks = mCallbacks.find(type);
        if (typeCallbacks == mCallbacks.end()) continue;

        auto callbacksSet = (*typeCallbacks).second;
        for (auto it = callbacksSet.begin(); it != callbacksSet.end(); it++) {
            if ((*it)->wakeupTimestamp <= wakeupTime) {
                ((*it)->callback)(frameId, wakeupTime);
                callbacksSet.erase(*it);
            }
        }
    }
}
