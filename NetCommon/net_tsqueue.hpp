#pragma once
#include "net_common.hpp"

namespace olc {

    namespace net {

        template<typename T>
        class tsqueue {
        public: 
            // default constructor
            tsqueue() = default;
            // not allow the queue to be copied
            tsqueue(const tsqueue<T>&) = delete;
            // destructor that uses the clear function
            virtual ~tsqueue() { clear(); }

        public: 
            // Returns and maintains item at front of Queue
            const T& front() {
                std::scoped_lock lock(muxQueue);
                return deqQueue.front();
            }

            // Return and maintains item at the back of the queue
            const T& back() {
                std::scoped_lock lock(muxQueue);
                return deqQueue.back();
            }

            // Removes and returns item from front of queue
            T pop_front() {
                std::scoped_lock lock(muxQueue);
                auto cached_front = std::move(deqQueue.front());
                deqQueue.pop_front();
                return cached_front;
            }

            // Removes and returns item from back of queue
            T pop_back() {
                std::scoped_lock lock(muxQueue);
                auto cached_back = std::move(deqQueue.back());
                deqQueue.pop_back();
                return cached_back;
            }

            // Adds an item to back of the Queue
            void push_back(const T& item){
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_back(std::move(item));

                std::unique_lock<std::mutex> ul(muxBlocking);
                cvBlocking.notify_one();
            }

            // Adds an item to the front of the Queue
            void push_front(const T& item) {
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_front(std::move(item));

                std::unique_lock<std::mutex> ul(muxBlocking);
                cvBlocking.notify_one();
            }
            
            // Returns true if the Queue is empty
            bool empty() {
                std::scoped_lock lock(muxQueue);
                return deqQueue.empty();
            }

            // Return number of items in Queue
            size_t count() {
                std::scoped_lock lock (muxQueue);
                return deqQueue.size();
            }

            // Clears Queue
            void clear() {
                std::scoped_lock lock(muxQueue);
                deqQueue.clear();
            }

            void wait() {
                while (empty()) {
                    std::unique_lock<std::mutex> ul(muxBlocking);
                    cvBlocking.wait(ul);
                }
            }
            

            

        protected:
            // mutex protects shared data(the double ended que - deqQueue) to be simulateniously accessed from multiple sources
            std::mutex muxQueue;
            // double ended queue
            std::deque<T> deqQueue;
            std::condition_variable cvBlocking;
            std::mutex muxBlocking;
        };
    }
}