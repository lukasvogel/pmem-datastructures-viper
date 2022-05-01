//
// Created by Lukas Vogel on 10.03.2021.
//

#ifndef LSM_PIBENCHWRAPPER_H
#define LSM_PIBENCHWRAPPER_H

#include "pibench/tree_api.h"
#include "viper/viper.hpp"
#include <mutex>
static thread_local viper::Viper<uint64_t, uint64_t>::Client client;


class PibenchWrapper : public tree_api {

public:
  inline bool find(const char* key, size_t key_sz, char* value_out) override;
  inline bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
  inline bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
  inline bool remove(const char* key, size_t key_sz) override;
  inline int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

  inline long get_size() override;
  explicit PibenchWrapper() {
    viper_db = viper::Viper<uint64_t, uint64_t>::create("/mnt/pmem0/vogel/viper", 200ul * 1024 * 1024 * 1024);
  }

private:


  std::unique_ptr<viper::Viper<uint64_t, uint64_t>> viper_db;


};


#endif //LSM_PIBENCHWRAPPER_H
