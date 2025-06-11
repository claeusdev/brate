#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

template <typename T> class Brate {
  queue<T> q_;
  mutable mutex lock_;
  condition_variable cv;

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
    std::unique_lock<mutex> lock(lock_);
    cv.wait(lock,
            [this] { return !q_.empty(); }); // wait till current queue is empty
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
