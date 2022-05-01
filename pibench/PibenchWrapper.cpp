//
// Created by Lukas Vogel on 10.03.2021.
//

#include <cassert>
#include <shared_mutex>
#include "PibenchWrapper.h"



extern "C" tree_api* create_tree(const tree_options_t& opt) {
    assert(opt.key_size == 8);
    assert(opt.value_size == 8);

    return new PibenchWrapper();
}

bool PibenchWrapper::remove(const char *key, size_t key_sz) {
    uint64_t int_key = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    if (!client.is_connected) {
        client = viper_db->get_client();
    }
    return client.remove(int_key);
    //return true;
}

bool PibenchWrapper::find(const char *key, size_t key_sz, char *value_out) {
    uint64_t int_key = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    if (!client.is_connected) {
        client = viper_db->get_client();
    }

    client.get(int_key, reinterpret_cast<uint64_t*>(const_cast<char*>(value_out)));
    return true;
}

bool PibenchWrapper::insert(const char *key, size_t key_sz, const char *value, size_t value_sz) {
    uint64_t int_key = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));

    //std::unique_lock<std::mutex> lock(mutexes.at(int_key % num_threads));

    if (!client.is_connected) {
        client = viper_db->get_client();
    }
    return client.put(int_key, *reinterpret_cast<uint64_t*>(const_cast<char*>(value)));
}

bool PibenchWrapper::update(const char *key, size_t key_sz, const char *value, size_t value_sz) {
    return false;
}

int PibenchWrapper::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) {
    return 0;
}
long PibenchWrapper::get_size() {
  return viper_db->v_base_.v_metadata->num_used_blocks * 6 * 4 * 1024;
}
