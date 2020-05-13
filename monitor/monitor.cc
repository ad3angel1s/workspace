#include <mutex>
#include <queue>
#include <cstdio>
#include <condition_variable>
#include <thread>
#include <chrono>

/*
 * A condition variable is a container of associated threads waiting for a
 * condition to evaluate to to true. In addition with a mutex it is used
 * to form a locking mechanism known as monitor. Example application is to
 * solve the producer-consumer problem.
 *
 * Suppose I have a queue where one or more threads add items, and one or
 * more threads remove items. The operations on the queue, i.e. pop() and
 * push(), consistute a critical section as they may leave the queue in a
 * unconsistent state, so it needs to be protected by a mutual exclusion
 * mechanism:
 *
 * void push(T *item) {
 *   lock(_mutex);
 *   while (queue.isFull()) {};
 *   queue.enqueue(item);
 *   unlock(_mutex);
 * }
 *
 * T *pop() {
 *   T *item = NULL;
 *   lock(_mutex);
 *   while (queue.isEmpty()) {};
 *   queue.dequeue(&item);
 *   unlock(_mutex);
 *   return item;
 * }
 *
 * This solution protects the critical section but obviously it will not work
 * as the active busy waiting loop in the critical section will prevent other
 * threads from unblocking the threads in the active wait, i.e. this solution
 * deadlocks.
 *
 * The easiest fix is to keep the busy waiting but allow some time for other
 * threads to acquire the lock to push or pop items, i.e.
 *
 * void push(T *item) {
 *   lock(_mutex);
 *   while (queue.isFull()) {
 *     unlock(_mutex);
 *     ... // some random time wait
 *     lock(_mutex);
 *   }
 *   queue.enqueue(item);
 *   unlock(_mutex);
 * }
 *
 * This solution works to resolve the deadlock, but introduce a waste of
 * resources in the busy wait which keeps unlocking and relocking the mutex.
 * Mutex locking and unlocking involves most of the time accessing some OS
 * resources, e.g. futex() system call. We need a mechanism to both provide
 * mutual exclusion and conditionally wait on a condition to happen.
 *
 * void push(T *item) {
 *   lock(_mutex);
 *   while(queue.isFull()) {
 *     wait(_mutex, isFullCondVar);
 *   }
 *   queue.enqueue(item);
 *   notify_one(isEmptyCondVar);
 *   unlock(_mutex);
 * }
 *
 * The wait() operation on the conditional variable happens atomically, and
 * needs to receive a locked mutex; What it does is:
 * 1. Releases the mutex
 * 2. Puts the thread on the WAITING list for the conditional variable
 *    associated, i.e. the thread goes to sleep (WAIT)
 * When the conditional variable is notified, the OS chooses one or all threads
 * sleeping and puts them back in the RUNNING queue with the mutex assigned.
 * The threads needs to check again the predicate as that might have changed
 * again by the time they were notified, i.e. other threads might have already
 * enqueue one or more items, making the queue full again. As we add one item,
 * we notify then the other conditional variable saying that one thread from
 * the waiting list could be resumed as there is an item now in the queue.
 * Then we unlock. Note that in case of notify_all(), all threads go back to
 * the running queue, but as only one thread is guaranteed to have the mutex
 * lock when it returns from wait(), other threads might end up waiting on the
 * mutex as part of the return path from the wait().
 *
 * Similarly happens on the pop() side:
 *
 * T *pop() {
 *   T *item = NULL;
 *   lock(_mutex);
 *   while(queue.isEmpty()) {
 *     wait(_mutex, isEmptyCondVar);
 *   }
 *   queue.dequeue(&item);
 *   notify_one(isFullCondVar);
 *   unlock(_mutex);
 *   return item;
 * }
 *
 * Reminder: A mutex can be implemented using a read-modify-write atomic exposed
 * by the ISA such as the test-and-set, which is a function that sets to 1 a mem
 * location and returns the old value (i.e. in kernel code to do a proper sleep
 * and not a busy wait):
 *
 * void Lock() {
 *   extern unsigned lock;
 *   while (test-and-set(lock) == 1) {}; // If it was 0 it would have passed
 * }
 *
 * void Unlock() {
 *   extern unsigned lock;
 *   lock = 0;
 * }
 *
 * Other possible ways of doing a read-modify-write atomically: 
 * test-and-set(void *loc) / compare-and-swap(void *loc, int old, int new)
 */

template<typename T, size_t Q_SIZE>
class Queue {
  private:
    std::queue<T> _queue;
  public:
    bool enqueue(T item) {
      if (_queue.size() < Q_SIZE) {
        _queue.push(item);
        return true;
      } else {
        return false;
      }
    };

    T dequeue(void) {
      T item = _queue.front();
      _queue.pop();
      return item;
    };

    unsigned size(void) { return _queue.size(); };
    bool empty(void) { return _queue.empty(); };
    bool full(void) {return (_queue.size() == Q_SIZE ? true : false); };
};

template<typename T, size_t Q_SIZE>
class SafeQueue {
  private:
    Queue<T, Q_SIZE> queue;
    std::mutex _mutex;
    std::condition_variable isEmptyCondVar;
    std::condition_variable isFullCondVar;
  public:
    bool push(T item);
    T pop(void);
};

template<typename T, size_t Q_SIZE>
bool SafeQueue<T, Q_SIZE>::push(T item) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII style acquisition using the unique_lock wrapper
  while (queue.full()) {
    isFullCondVar.wait(lock);
  }
  queue.enqueue(item);
  isEmptyCondVar.notify_one();
  return true;
}

template<typename T, size_t Q_SIZE>
T SafeQueue<T, Q_SIZE>::pop(void) {
  std::unique_lock<std::mutex> lock(_mutex);
  while (queue.empty()) {
    isEmptyCondVar.wait(lock);
  }
  T item = queue.dequeue();
  isFullCondVar.notify_one();
  return item;
}

int main(void) {
  Queue<int,10> queue;
  unsigned count = 0;

  printf("Queue size is %u and empty: %u, full: %u\n", queue.size(), queue.empty(), queue.full());
  while (queue.enqueue(++count)) {};
  printf("Queue size is %u and empty: %u, full: %u\n", queue.size(), queue.empty(), queue.full());

  SafeQueue<int,10> sq;
  std::thread t1([&]() {
    for (unsigned i = 0; i<20; i++) {
      sq.push(i);
      printf("Pushed element %u\n", i);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  std::thread t2([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(7500));
    for (unsigned i = 0; i<20; i++) {
      printf("Popped element %u\n", sq.pop());
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  t1.join();
  t2.join();
  return 0;
}
