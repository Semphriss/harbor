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

#include "gtest/gtest.h"

#include "scripting/scriptable.hpp"

#define private public
#define protected public
#define final

#include "scripting/virtual_machine.hpp"

#undef private
#undef protected
#undef final

#define COMMA ,
#define COMMA1 COMMA

template<typename T, typename V>
void
test_type()
{
  auto bound = VMUtils::bind("", std::function<void(T)>([](T){}));
  std::vector<std::unique_ptr<VMUtils::Type>> args;
  args.push_back(std::make_unique<V>());
  bound.m_function(args);
}

TEST(Scripting_VirtualMachine_SingleArg, parse)
{
  ASSERT_NO_THROW((test_type<bool, VMUtils::Boolean>()));
  ASSERT_NO_THROW((test_type<float, VMUtils::Float>()));
  ASSERT_NO_THROW((test_type<int, VMUtils::Integer>()));
  ASSERT_NO_THROW((test_type<Scriptable*, VMUtils::Object>()));
  ASSERT_NO_THROW((test_type<std::string, VMUtils::String>()));

  ASSERT_THROW((test_type<bool, VMUtils::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VMUtils::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VMUtils::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VMUtils::String>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VMUtils::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VMUtils::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VMUtils::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VMUtils::String>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VMUtils::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VMUtils::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VMUtils::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VMUtils::String>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VMUtils::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VMUtils::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VMUtils::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VMUtils::String>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VMUtils::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VMUtils::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VMUtils::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VMUtils::Object>()), std::runtime_error);
}
