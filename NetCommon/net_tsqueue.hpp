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
            tsqueue(tsqueue<T> const&) = delete;
            // destructor that uses the clear function
            virtual ~tsqueue() { clear(); }

            // Returns and maintains item ath front of Queue
            T const& front() {
                std::scoped_lock lock(muxQueue);
                return deqQueue.front();
            }

            // Return and maintains item at the back of the queue
            T const& back() {
                std::scoped_lock lock(muxQueue);
                return deqQueue.back();
            }

            // Adds an item to back of the Queue
            void push_back(T const& item){
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_back(std::move(item));
            }

            // Adds an item to the fron of the Queue

            void push_front(T const& item) {
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_front(std::move(item));
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

            void clear() {
                std::scoped_lock lock(muxQueue);
                deqQueue.clear();
            }

            // Removes and returns item from front of queue
            T pop_front() {
                std::scoped_lock lock(muxQueue);
                auto temp = std::move(deqQueue.front());
                deqQueue.pop_front();
                return temp;
            }

            // Removes and returns item from back of queue
            T pop_back() {
                std::scoped_lock lock(muxQueue);
                auto temp = std::move(deqQueue.back());
                deqQueue.pop_back();
                return temp;
            }

            protected:
                // mutex protects shared data to be simulateniously accessed from multiple sources
                std::mutex muxQueue;
                // double ended queue
                std::deque<T> deqQueue;
        };
    }
}