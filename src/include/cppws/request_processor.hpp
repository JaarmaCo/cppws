#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory_resource>
#include <mutex>
#include <thread>

#include <cppws/http_request.hpp>
#include <cppws/socket.hpp>

namespace cppws {

/**
 * Context for handling requests
 */
class request_manager;

/**
 * Maps URL endpoints to request handlers.
 */
class request_mapper {
public:
  using handler = std::function<void(request_manager &)>;

  virtual ~request_mapper() noexcept {}

  /**
   * \brief Resolve a handler for the given request.
   *
   * \param request Request to resolve a handler for.
   * \return A functor that will be used to handle the request, or an empty
   * std::function if a handler did not exist.
   */
  virtual handler resolve(const http_request &request) = 0;
};

/**
 * Encapsulates a thread that processes HTTP requests.
 */
class request_processor {
public:
  /**
   * \brief Constructs a new request processor that starts waiting for requests.
   *
   * \param mapper Pointer to a request_mapper that is used to resolve handlers
   * for the requests.
   * \param upstream Allocator used by the processor.
   */
  explicit request_processor(
      std::shared_ptr<request_mapper> mapper,
      std::pmr::memory_resource *upstream = std::pmr::get_default_resource());

  /**
   * \brief Accept a new connection on the given socket.
   *
   * \param socket Socket connection to accept.
   */
  void accept(socket &&socket);

  /**
   * \brief Signal to the processor that request handling is to stop.
   */
  void terminate();

  /**
   * \brief True when the processor thread is in a non-exited state.
   */
  bool active() const noexcept;

  /**
   * \brief True when the processor is processing a request.
   */
  bool busy() const noexcept;

  /**
   * \brief Blocks execution of the current thread until the processor becomes
   * available.
   *
   * \param timeout Maximum amount of time to wait for.
   */
  void wait_until_available(const std::chrono::milliseconds &timeout);

private:
  void run();
  void process_request();

  std::pmr::monotonic_buffer_resource buffer_;

  std::atomic_bool running_;
  std::atomic_bool busy_;

  std::mutex lock_;
  std::condition_variable availableCondition_;
  socket connection_;
  http_request processedRequest_;
  std::thread::id runner_;

  std::shared_ptr<request_mapper> mapper_;
};

} // namespace cppws
