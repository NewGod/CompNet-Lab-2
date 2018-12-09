#ifndef __SAFEQUEUE_H__
#define __SAFEQUEUE_H__
#include<queue>
#include<mutex>
#include<condition_variable> 
#include<chrono>
#include<map>
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
    T get(std::chrono::seconds timeout = std::chrono::seconds(1)) {
		
        std::unique_lock<std::mutex> lock(m);
		if (q.empty()){
			if (cv.wait_for(lock, timeout) == std::cv_status::timeout)
				throw "SafeQueue Error: Pop Timeout";
			else {
				T ret = q.front();
				q.pop();
				return ret;
            }
		}else {
			T ret = q.front();
			q.pop();
			return ret;
		}
    }
};
template<class T1, class T2>
class SafeMap{ 
	std::map<T1, T2> buf;
	std::map<T1, std::condition_variable*> request; 
    std::mutex m, rm; 
	public:
	T2 Get(T1 key){
        std::lock_guard<std::mutex> lock(m); 
		return buf[key];
	}
	void Set(T1 key, T2 item){
        std::lock_guard<std::mutex> lock(m); 
		buf[key] = item; 
	}
	void Notify(T1 key){
		std::condition_variable* cv;
		{
			std::lock_guard<std::mutex> lock(rm); 
			cv = request[key];
		}
		if (cv) cv->notify_all();
	}
	T2 Request(T1 target, bool test(T2), std::chrono::seconds sec = std::chrono::seconds(1)){
		{
			std::lock_guard<std::mutex> lock(m); 
			if (test(buf[target])) return buf[target]; 
		}
		std::mutex mtx;
		std::unique_lock<std::mutex> lck(mtx);
		std::condition_variable *cv;

		{ 
			std::lock_guard<std::mutex> lock(rm); 
			if (request[target]) cv = request[target];
			else{
				cv = new std::condition_variable();
				request[target] = cv;
			}
		}
		cv->wait_for(lck,sec);
		{
			std::lock_guard<std::mutex> lock(m); 
			if (test(buf[target])) return buf[target];
		}
		throw "safeRequest: Request Timeout";
	}
};
#endif
