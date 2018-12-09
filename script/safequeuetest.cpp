#include "safequeue.h" 
#include<thread>
#include<iostream>
#include<mutex>
#include<unistd.h>
SafeQueue<int> q; 
std::mutex m;
void read_value(int x) {
    int ret = q.get();
    std::lock_guard<std::mutex> lock(m);
    std::cout<<"thread: "<<x<<" value: "<<ret << std::endl;
}
std::thread th[10];
int main(){
    for (int i=0;i<10;i++) 
        th[i] = std::thread(read_value, i);
	sleep(1);
    for (int i=0;i<10;i++) 
        q.push(i);
    for (int i=0;i<10;i++) th[i].join();
    return 0;
}
