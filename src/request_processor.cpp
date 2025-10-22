#include <cppws/http_def.hpp>
#include <cppws/http_response.hpp>
#include <cppws/request_processor.hpp>
#include <cppws/socket_stream.hpp>

cppws::request_processor::request_processor(
    std::shared_ptr<request_mapper> mapper, std::pmr::memory_resource *upstream)
    : mapper_(mapper), buffer_(upstream) {
  std::thread thread([this]() { run(); });
  runner_ = thread.get_id();
  thread.detach();
}

bool cppws::request_processor::accept(socket &&connection) {

  if (!wait_until_available())
    return false;

  {
    std::unique_lock l{lock_};

    stream_ = pmr::socket_iostream{
        pmr::socket_streambuf(std::move(connection), &buffer_)};
    if (!http_request::accept(processedRequest_, stream_))
      return false; // Bad format

    hasRequest_ = true;

    // connection made, notify handler
    //
    newConnectionCondition_.notify_one();
  }
  return true;
}

void cppws::request_processor::terminate() { running_ = false; }

bool cppws::request_processor::active() const noexcept { return running_; }

bool cppws::request_processor::busy() const noexcept { return busy_; }

bool cppws::request_processor::wait_until_available(
    const std::chrono::milliseconds &timeout) {

  using clock = std::chrono::high_resolution_clock;
  using time_point = clock::time_point;

  if (!running_)
    return false;

  std::unique_lock l{lock_};
  time_point dl = clock::now() + timeout;
  while (busy_) {
    if (availableCondition_.wait_until(l, dl) == std::cv_status::timeout)
      return false;
  }
  return true;
}

void cppws::request_processor::run() {
  while (running_) {

    // Await new connection
    {
      std::unique_lock l{lock_};
      if (!hasRequest_) {
        newConnectionCondition_.wait(l);
        if (!hasRequest_)
          continue;
      }
    }

    process_request();

    // Lock and notify available
    {
      std::unique_lock l{lock_};
      availableCondition_.notify_all();
    }
  }
}

void cppws::request_processor::process_request() {

  try {
    request_mapper::handler handler = mapper_->resolve(processedRequest_);
    if (!handler) {
      stream_ << http::FORBIDDEN << http::body("Entry blocked by filter.");
      throw std::exception();
    }
    try {
      request_manager manager;
      handler(manager);
    } catch (...) {
      stream_ << http::INTERNAL_SERVER_ERROR
              << http::body("An unexpected internal server error occured.");
    }
  } catch (...) {
  }
  stream_ = {};
}
