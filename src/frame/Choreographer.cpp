#include "Choreographer.h"

static std::shared_ptr<Choreographer> sInstance = nullptr;

int Choreographer::registerVSyncReceiver(std::unique_ptr<VSyncReceiver> receiver) {
    return 0;
}

int Choreographer::unregisterVSyncReceiver(std::unique_ptr<VSyncReceiver> receiver) {
    return 0;
}

Choreographer::Choreographer() {
    mVSyncDispatcher = vsync::VSyncDispatcher::createVSyncDispatcher();
}

void Choreographer::run() {

}

std::shared_ptr<Choreographer> Choreographer::getInstance() {
    if (sInstance == nullptr) {
        sInstance = std::make_shared<Choreographer>();
    }
    return sInstance;
}