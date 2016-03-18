#ifndef __CONCURRENTSET_HPP_INCLUDED__
#define __CONCURRENTSET_HPP_INCLUDED__

#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>
 
template <typename T> class ConcurrentSet
{
public:

  ConcurrentSet(std::unordered_set<T>& source) : set_(source) {};

  std::unique_lock<std::mutex> get_lock()
  {
    return std::unique_lock<std::mutex>(this->mutex_);
  }

  void insert(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    set_.insert(item);
    mlock.unlock();
    cond_.notify_one();
  }

  void insert(T&& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    set_.insert(std::move(item));
    mlock.unlock();
    cond_.notify_one();
  }

  bool insert_atomic(const T& item)
  {
    bool result = false;
    std::unique_lock<std::mutex> mlock(mutex_);
    if(this->count_unsafe(item))
    {
      result = false;
    } else {
      result = true;
      this->insert_unsafe(item);
      cond_.notify_one();
    }
    mlock.unlock();
    return result;
  }

  bool insert_atomic(T&& item)
  {
    bool result = false;
    std::unique_lock<std::mutex> mlock(mutex_);
    if(this->count_unsafe(item))
    {
      result = false;
    } else {
      result = true;
      this->insert_unsafe(item);
      cond_.notify_one();
    }
    mlock.unlock();
    return result;
  }

  size_t count(const T& item)
  {
    size_t result = 0;
    std::unique_lock<std::mutex> mlock(mutex_);
    result = set_.count(item);
    mlock.unlock();
    return result;
  }

  size_t count_unsafe(const T& item)
  {
    return set_.count(item);
  }

  void insert_unsafe(const T& item)
  {
    set_.insert(item);
    cond_.notify_one();
  }

  void insert_unsafe(T&& item)
  {
    set_.insert(std::move(item));
    cond_.notify_one();
  }

  private:
    std::unordered_set<T>& set_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif