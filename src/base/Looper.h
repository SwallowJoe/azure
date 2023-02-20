#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Time.h"

struct Message {
  Message() : what(0) {}
  Message(int w) : what(w) {}
  int what;
};

class MessageHandler {
 protected:
  virtual ~MessageHandler() = 0;

 public:
  /**
   * @brief Handle a message
   *
   */
  virtual void handleMessage(const Message&) = 0;
};

class Looper final {
 public:
  Looper();
  ~Looper();
  void sendMessage(const std::shared_ptr<MessageHandler> handler, const Message&);
  void sendMessageDelay(const std::shared_ptr<MessageHandler> handler, const Message&, int millisecond);
  void removeMessages(int);

 private:
  bool threadMain(std::unique_lock<std::mutex>&);

  std::thread mThread;
  mutable std::mutex mMutex;
  mutable std::condition_variable mCondition;

  struct MessageEntry {
    nsecs_t uptime;
    Message message;
    std::shared_ptr<MessageHandler> handler;
  };

  // store all messages.
  std::vector<MessageEntry> mMessageEntries;
  const char* const mThreadName;
  bool mThreadRunning = true;
};