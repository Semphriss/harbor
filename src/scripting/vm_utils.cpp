//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2021 Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// CATASTROPHIC FAILURE
// In C++, there will always be a way to do anything.
// However, it's not guaranteed that that way was actually intended.

#ifndef _HEADER_HARBOR_SCRIPTING_VMUTILS_HPP

#include "scripting/vm_utils.hpp"

#else

#include "function_tuple_args.hpp"
#include "make_unique.hpp"

template<size_t Pos, typename... Args>
void
VMUtils::SingleArg<bool, Pos, Args...>::parse(std::tuple<Args...>& tuple, VMUtils::Type* arg)
{
  const auto i = dynamic_cast<const VMUtils::Boolean*>(arg);

  if (!i)
  {
    throw std::runtime_error("Argument " + std::to_string(Pos + 1) + " must be a boolean");
  }

  std::get<Pos>(tuple) = i->m_value;
}

template<size_t Pos, typename... Args>
void
VMUtils::SingleArg<float, Pos, Args...>::parse(std::tuple<Args...>& tuple, VMUtils::Type* arg)
{
  const auto i = dynamic_cast<const VMUtils::Float*>(arg);

  if (!i)
  {
    throw std::runtime_error("Argument " + std::to_string(Pos + 1) + " must be a float");
  }

  std::get<Pos>(tuple) = i->m_value;
}

template<size_t Pos, typename... Args>
void
VMUtils::SingleArg<int, Pos, Args...>::parse(std::tuple<Args...>& tuple, VMUtils::Type* arg)
{
  const auto i = dynamic_cast<const VMUtils::Integer*>(arg);

  if (!i)
  {
    throw std::runtime_error("Argument " + std::to_string(Pos + 1) + " must be an integer");
  }

  std::get<Pos>(tuple) = i->m_value;
}

template<size_t Pos, typename... Args>
void
VMUtils::SingleArg<Scriptable*, Pos, Args...>::parse(std::tuple<Args...>& tuple, VMUtils::Type* arg)
{
  const auto i = dynamic_cast<const VMUtils::Object*>(arg);

  if (!i)
  {
    throw std::runtime_error("Argument " + std::to_string(Pos + 1) + " must be a class");
  }

  std::get<Pos>(tuple) = i->m_value;
}

template<size_t Pos, typename... Args>
void
VMUtils::SingleArg<std::string, Pos, Args...>::parse(std::tuple<Args...>& tuple, VMUtils::Type* arg)
{
  const auto i = dynamic_cast<const VMUtils::String*>(arg);

  if (!i)
  {
    throw std::runtime_error("Argument " + std::to_string(Pos + 1) + " must be a string");
  }

  std::get<Pos>(tuple) = i->m_value;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<void, A...>::exec(std::function<void(A...)> func, const std::tuple<A...>& args)
{
  call(func, args);
  return nullptr;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<bool, A...>::exec(std::function<bool(A...)> func, const std::tuple<A...>& args)
{
  auto ret = std::make_unique<VMUtils::Boolean>();
  ret->m_value = call(func, args);
  return ret;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<float, A...>::exec(std::function<float(A...)> func, const std::tuple<A...>& args)
{
  auto ret = std::make_unique<VMUtils::Float>();
  ret->m_value = call(func, args);
  return ret;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<int, A...>::exec(std::function<int(A...)> func, const std::tuple<A...>& args)
{
  auto ret = std::make_unique<VMUtils::Integer>();
  ret->m_value = call(func, args);
  return ret;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<Scriptable*, A...>::exec(std::function<Scriptable*(A...)> func, const std::tuple<A...>& args)
{
  auto ret = std::make_unique<VMUtils::Object>();
  ret->m_value = call(func, args);
  return ret;
}

template<typename... A>
std::unique_ptr<VMUtils::Type>
VMUtils::ReturnVal<std::string, A...>::exec(std::function<std::string(A...)> func, const std::tuple<A...>& args)
{
  auto ret = std::make_unique<VMUtils::String>();
  ret->m_value = call(func, args);
  return ret;
}

template<std::size_t Current, std::size_t End, typename... Args>
VMUtils::FetchArg<Current, End, Args...>::FetchArg(std::tuple<Args...>& tuple, const std::vector<std::unique_ptr<VMUtils::Type>>& args)
{
  SingleArg<typename std::tuple_element<Current, std::tuple<Args...>>::type, Current, Args...>::parse(tuple, args[Current].get());

  FetchArg<Current + 1, End, Args...>(tuple, args);
}

template<std::size_t Current, std::size_t End, typename... Args>
void
VMUtils::ListArg<Current, End, Args...>::list(std::vector<VMUtils::Types>& list)
{
  if (std::is_same<typename std::tuple_element<Current, std::tuple<Args...>>::type, bool>::value)
  {
    list.push_back(VMUtils::Types::BOOLEAN);
  }
  else if (std::is_same<typename std::tuple_element<Current, std::tuple<Args...>>::type, int>::value)
  {
    list.push_back(VMUtils::Types::INTEGER);
  }
  else if (std::is_same<typename std::tuple_element<Current, std::tuple<Args...>>::type, float>::value)
  {
    list.push_back(VMUtils::Types::FLOAT);
  }
  else if (std::is_same<typename std::tuple_element<Current, std::tuple<Args...>>::type, Scriptable*>::value)
  {
    list.push_back(VMUtils::Types::OBJECT);
  }
  else if (std::is_same<typename std::tuple_element<Current, std::tuple<Args...>>::type, std::string>::value)
  {
    list.push_back(VMUtils::Types::STRING);
  }
  else
  {
    throw std::runtime_error("Unrecognized C++ argument type #" + std::to_string(Current + 1));
  }

  ListArg<Current + 1, End, Args...>::list(list);
}

template<typename R, typename... A, size_t NArgs>
VMUtils::Function
VMUtils::bind(const std::string& name, std::function<R(A...)> func)
{
  auto function = [func](const std::vector<std::unique_ptr<VMUtils::Type>>& args) -> std::unique_ptr<VMUtils::Type> {

    if (args.size() != std::tuple_size<std::tuple<A...>>::value)
    {
      throw std::runtime_error("Wrong number of arguments (expected "
                      + std::to_string(std::tuple_size<std::tuple<A...>>::value)
                      + ", got " + std::to_string(args.size()) + ")");
    }

    std::tuple<A...> native_args;

    FetchArg<0, std::tuple_size<std::tuple<A...>>::value, A...>(native_args, args);

    return ReturnVal<R, A...>::exec(func, native_args);
  };

  std::vector<VMUtils::Types> list;
  ListArg<0, std::tuple_size<std::tuple<A...>>::value, A...>::list(list);

  return {name, function, list};
}

#endif
