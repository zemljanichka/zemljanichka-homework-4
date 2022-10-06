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

bool phone_book_t::is_prefix(const std::string &prefix, const std::string &name) const {
  if (prefix.size() > name.size())
    return false;
  auto it = prefix_names.find(name);
  if (it == prefix_names.end())
    return false;
  auto current_prefix = it->second.find(prefix);
  return current_prefix != it->second.end();
}

void phone_book_t::add_prefixes(const std::string &str) {
  std::string new_prefix;
  prefix_names[str].insert(new_prefix);
  for (auto chr : str){
    new_prefix += chr;
    prefix_names[str].insert(new_prefix);
  }
}
