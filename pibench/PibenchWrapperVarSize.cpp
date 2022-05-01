//
// Created by Lukas Vogel on 10.03.2021.
//

#include <cassert>
#include <shared_mutex>
#include "PibenchWrapperVarSize.h"



extern "C" tree_api* create_tree(const tree_options_t& opt) {
  assert(opt.key_size == 8);
  assert(opt.value_size == 8);

  return new PibenchWrapperVarSize();
}

bool PibenchWrapperVarSize::remove(const char *key, size_t key_sz) {
  std::string str_key(key, key_sz);
  if (!client.is_connected) {
    client = viper_db->get_client();
  }
  return client.remove(str_key);
}

bool PibenchWrapperVarSize::find(const char *key, size_t key_sz, char *value_out) {
  std::string str_key(key, key_sz);

  if (!client.is_connected) {
    client = viper_db->get_client();
  }
  std::string result;

  bool found = client.get(str_key, &result);
  memcpy(value_out, result.data(), result.size());

  return found;
}

bool PibenchWrapperVarSize::insert(const char *key, size_t key_sz, const char *value, size_t value_sz) {
  std::string str_key(key, key_sz);
  std::string str_val(value, value_sz);


  if (!client.is_connected) {
    client = viper_db->get_client();
  }
  client.put(str_key, str_val);
  return true;
}

bool PibenchWrapperVarSize::update(const char *key, size_t key_sz, const char *value, size_t value_sz) {
  return false;
}

int PibenchWrapperVarSize::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) {
  return 0;
}

long PibenchWrapperVarSize::get_size() {
  return viper_db->v_base_.v_metadata->num_used_blocks * 6 * 4 * 1024;
}

