/*
 * SessionManager.cpp
 *
 *  Created on: 2017年12月4日
 *      Author: xueda
 */

#include <glog/logging.h>
#include "Session.h"
#include "SessionManager.h"

namespace network {

SessionManager::SessionManager()
    : running_(true),
      monitor_(nullptr) {
  monitor_ = new std::thread(&SessionManager::MonitorThread, this);
  CHECK_NOTNULL(monitor_);
}

SessionManager::~SessionManager() {
  running_ = false;
  monitor_->join();
  delete monitor_;
  monitor_ = nullptr;
  usleep(100 * 1000);
  DLOG(INFO)<< __FUNCTION__;
}

void SessionManager::Stop() {
  running_ = false;
}

void SessionManager::AddSession(const std::shared_ptr<Session>& session) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (sessions_.end() == sessions_.find(session->session_id())) {
    return;
  }
  sessions_[session->session_id()] = session;
  cond_var_.notify_one();
}

void SessionManager::DeleteSession(const std::string& session_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto iter = sessions_.find(session_id);
  if (iter != sessions_.end()) {
    sessions_.erase(iter);
  }
  cond_var_.notify_one();
}

std::shared_ptr<Session> SessionManager::GetSession(
    const std::string& session_id) {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_var_.wait(lock, [this] {return !sessions_.empty();});
  auto iter = sessions_.find(session_id);
  if (iter != sessions_.end()) {
    return sessions_[session_id];
  }
  return nullptr;
}

void SessionManager::MonitorThread() {
  while (running_) {
    // FIXME : No implement now
    usleep(500 * 1000);
  }
}

} /* namespace network */

