#include "device.h"

#include <list>
#include <memory>

static int dev_cnt;
static std::list<std::unique_ptr<Device>> list;

Device::Device(const std::string &name) :
    name(name),
    id(dev_cnt++) {}    


int addDevice(const std::string &name) {
    if (findDevice(name)) return -1;
    list.push_back(std::make_unique<Device>(name));    
    return list.back()->id;
}
Device* findDevice(const std::string &name) {
	for (auto &i : list)
        if (i->name == name)
            return i.get();
    return NULL;
}
Device *getDevice(int id) {
    for (auto &i : list)
        if (i->id == id)
            return i.get();
    return NULL;
}
void eachDevice(const std::function<void(Device*)> &callback) {
    for (auto &i : list)
        callback(i.get());
}