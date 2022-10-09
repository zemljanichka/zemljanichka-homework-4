#include "phone-book.h"

bool phone_book_t::create_user(const std::string &number, const std::string &name)
{
  auto it = this->users.find(number);
  if(it != this->users.end())
    return false;
  this->users.insert({number, {name, 0}});
  add_prefixes(number);
  add_prefixes(name);
  this->users_list.insert({name, number});
  return true;
}

bool phone_book_t::add_call(const call_t &call)
{
  auto it = this->users.find(call.number);
  if(it == this->users.end())
    return false;
  this->calls.push_back(call);
  it->second.second += call.duration_s;
  return true;
}

std::vector<call_t> phone_book_t::get_calls(size_t start_pos, size_t count) const
{
  std::vector<call_t> result_calls;
  for(size_t i = start_pos; i < start_pos + count && i < this->calls.size(); i++)
  {
    result_calls.push_back(this->calls[i]);
  }
  return result_calls;
}

std::vector<user_info_t> phone_book_t::search_users_by_number(const std::string &number_prefix, size_t count) const
{
  std::vector<user_info_t> result_users;
  std::set <std::tuple <double, std::string, std::string>> tmp_res;

  auto current_user = users.lower_bound(number_prefix);

  while(current_user != this->users.end())
  {

    if(is_prefix(number_prefix, current_user->first))
    {
      tmp_res.insert({-current_user->second.second, current_user->second.first, current_user->first});
    }else{
      break;
    }
    current_user++;
  }
  int cnt = 0;
  for (auto user : tmp_res){
    if (cnt >= count)
      break;
    cnt++;
    user_info_t x;
    x.user.name = std::get<1>(user);
    x.user.number = std::get<2>(user);
    x.total_call_duration_s = -std::get<0>(user);
    result_users.push_back(x);
  }
  return result_users;
}

std::vector<user_info_t> phone_book_t::search_users_by_name(const std::string &name_prefix, size_t count) const
{
  std::vector<user_info_t> result_users;
  std::set <std::tuple <std::string, double, std::string>> tmp_res;

  auto current_user = users_list.lower_bound(name_prefix);

  while(current_user != this->users_list.end())
  {

    if(is_prefix(name_prefix, current_user->first))
    {
      auto user = users.find(current_user->second);
      tmp_res.insert({user->second.first, -user->second.second, user->first});
    }else{
      break;
    }
    current_user++;
  }
  int cnt = 0;
  for (auto user : tmp_res){
    if (cnt >= count)
      break;
    cnt++;
    user_info_t x;
    x.user.name = std::get<0>(user);
    x.user.number = std::get<2>(user);
    x.total_call_duration_s = -std::get<1>(user);
    result_users.push_back(x);
  }
  return result_users;
}

void phone_book_t::clear()
{
  this->users.clear();
  this->calls.clear();
  this->users_list.clear();
  this->prefix_names.clear();
}

size_t phone_book_t::size() const
{
  return this->users.size();
}

bool phone_book_t::empty() const
{
  return this->users.empty();
}

bool phone_book_t::is_prefix(const std::string &prefix, const std::string &str) const {
  if (prefix.size() > str.size())
    return false;

  std::vector <long long> prefix_hash;
  long long hash = 1ll;
  const long long HASH_MOD = 31ll;
  const long long MOD = 1e9+7ll;
  long long mod_greed = 1ll;
  prefix_hash.push_back(hash);
  for (char chr : prefix){
    hash = (hash + (long long)(chr - 'a' + 1) * mod_greed) % MOD;
    mod_greed = (HASH_MOD * mod_greed) % MOD;
//    hash += (long long)(chr - 'a' + 1ll) * mod_greed;
//    mod_greed *= HASH_MOD;
    prefix_hash.push_back(hash);
  }


  auto it_name = prefix_names.find(str);
  if (it_name == prefix_names.end())
    return false;
  auto current_prefix = it_name->second.at(prefix.size());

  return current_prefix == prefix_hash[prefix.size()];
}

void phone_book_t::add_prefixes(const std::string &str) {
  long long hash_str = 1ll;
  const long long HASH_MOD = 31ll;
  const long long MOD = 1e9+7ll;
  long long mod_greed = 1ll;
  prefix_names[str].push_back(hash_str);
  for (int i = 0 ; i < str.size(); i++){
    hash_str = (hash_str + (long long)(str[i] - 'a' + 1) * mod_greed) % MOD;
    mod_greed = (HASH_MOD * mod_greed) % MOD;
    //hash_str += (long long)(str[i] - 'a' + 1ll) * mod_greed;
    //mod_greed *= HASH_MOD;
    prefix_names[str].push_back(hash_str);
  }
}
