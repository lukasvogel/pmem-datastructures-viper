//
// Created by Lukas Vogel on 10.03.2021.
//

#ifndef LSM_PIBENCHWRAPPERVARSIZE_H
#define LSM_PIBENCHWRAPPERVARSIZE_H

#include "pibench/tree_api.h"
#include "viper/viper.hpp"
#include <mutex>
static thread_local viper::Viper<std::string, std::string>::Client client;


class PibenchWrapperVarSize : public tree_api {

public:
  inline bool find(const char* key, size_t key_sz, char* value_out) override;
  inline bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
  inline bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
  inline bool remove(const char* key, size_t key_sz) override;
  inline int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

  inline long get_size() override;
  explicit PibenchWrapperVarSize() {
    viper_db = viper::Viper<std::string, std::string>::create("/mnt/pmem0/vogel/viper", 200ul * 1024 * 1024 * 1024);
  }

private:


  std::unique_ptr<viper::Viper<std::string, std::string>> viper_db;


};


#endif //LSM_PIBENCHWRAPPERVARSIZE_H
