#ifndef __SAFEQUEUE_H__
#define __SAFEQUEUE_H__
#include<queue>
#include<mutex>
#include<condition_variable> 
#include<chrono>
template<class T>
class SafeQueue{
    std::queue<T> q;
    std::mutex m; 
    std::condition_variable cv;
    public: 
    void push(const T &elem) {
        std::lock_guard<std::mutex> lock(m); 
        q.push(elem);
        cv.notify_one();
    }
    T get(const std::chrono::milliseconds &timeout) {
        std::unique_lock<std::mutex> lock(m);
        if (q.empty()) {
            if (cv.wait_for(lock, timeout) == std::cv_status::no_timeout) {
                T ret = q.front();
                q.pop();
                return ret;
            }else throw "SafeQueue Error: Pop Timeout";
        }else {
            T ret = q.front();
            q.pop();
            return ret;
        }
    }
    T get() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this]{return !q.empty();});
        T ret = q.front();
        q.pop();
        return ret;
    }
};
#endif
