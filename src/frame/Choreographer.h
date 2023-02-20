#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "Time.h"
#include "AutoLock.h"
#include "VSyncDispatcher.h"

class VSyncReceiver {
public:
    virtual void onVSync(nsecs_t timestamp) = 0;
};

class Choreographer final {
public:

    Choreographer();
    ~Choreographer() = default;
    static std::shared_ptr<Choreographer> getInstance();

    int registerVSyncReceiver(std::unique_ptr<VSyncReceiver> receiver);
    int unregisterVSyncReceiver(std::unique_ptr<VSyncReceiver> receiver);
private:

    void run();

    mutable std::mutex mMutex;
    std::unordered_set<std::unique_ptr<VSyncReceiver>> mReceiverMap;
    std::shared_ptr<vsync::VSyncDispatcher> mVSyncDispatcher;
};
