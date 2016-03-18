#ifndef __CONCURRENTQUEUE_HPP_INCLUDED__
#define __CONCURRENTQUEUE_HPP_INCLUDED__

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 
template <typename T> class ConcurrentQueue
{
public:

  std::unique_lock<std::mutex> get_lock()
  {
    return std::unique_lock<std::mutex>(this->mutex_);
  }

  T pop()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    auto item = queue_.front();
    queue_.pop();
    return item;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }

  bool pop_nb(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    if(queue_.empty())
    {
      mlock.unlock();
      return false;
    }
    item = queue_.front();
    queue_.pop();
    mlock.unlock();
    return true;
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }

  void push(T&& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::move(item));
    mlock.unlock();
    cond_.notify_one();
  }

  void push_unsafe(const T& item)
  {
    queue_.push(item);
    cond_.notify_one();
  }

  void push_unsafe(T&& item)
  {
    queue_.push(std::move(item));
    cond_.notify_one();
  }

  bool empty()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    bool result = queue_.empty();
    mlock.unlock();
    return result;
  }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif