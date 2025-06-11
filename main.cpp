#include <conditional_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

// TODO:
// 1. Build a queue -> Done
// 2. make it thread safe -> Done
// 3. put it behind tcp socket
//
//
// interface/api?
//
// pop, push, try_pop, empty?
// top ?
//

template <typename T> class Brate {
  queue<T> q_;
  mutable mutex lock_;
  conditional_variable cv;

public:
  Brate() = default;

  // disable copying
  Brate<T>(const Brate<T> &) = delete;
  Brate &operator=(const Brate<T> &) = delete;

  // push an item
  void Push(const T &item) {
    // TODO: maybe use a unique lock
    std::lock_guard<mutex> lock(lock_);
    q_.push(item);
    cv.notify_one(); // notify waiting thread, if any
  }
  // pop an item
  T Pop() {
    // TODO: maybe use a unique lock
    std::lock_guard<mutex> lock(lock_);
    cv.wait(lock,
            [] { return q_.empty(); }); // wait till current queue is empty
    T item = q_.front();
    q_.pop();
    return item;
  }

  bool empty() const {
    lock_guard<mutex> lock(lock_);
    return q_.empty();
  }
};

int main() {
  // example usage
  Brate<int> bsq;

  std::thread producer([&bsq]() {
    for (int i = 0; i < 10; i++) {
      bsq.Push(i + 1);
    }
  });

  std::thread consumer([&bsq]() {
    for (int i = 0; i < 10; i++) {
      int value = bsq.Pop();
      cout << "current value: " << value << endl;
    }
  });

  producer.join();
  consumer.join();

  return 0;
}
