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
  auto bound = VirtualMachine::bind(std::function<void(T)>([](T){}));
  std::vector<std::unique_ptr<VirtualMachine::Type>> args;
  args.push_back(std::make_unique<V>());
  bound.first(args);
}

TEST(Scripting_VirtualMachine_SingleArg, parse)
{
  ASSERT_NO_THROW((test_type<bool, VirtualMachine::Boolean>()));
  ASSERT_NO_THROW((test_type<float, VirtualMachine::Float>()));
  ASSERT_NO_THROW((test_type<int, VirtualMachine::Integer>()));
  ASSERT_NO_THROW((test_type<Scriptable*, VirtualMachine::Object>()));
  ASSERT_NO_THROW((test_type<std::string, VirtualMachine::String>()));

  ASSERT_THROW((test_type<bool, VirtualMachine::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VirtualMachine::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VirtualMachine::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<bool, VirtualMachine::String>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VirtualMachine::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VirtualMachine::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VirtualMachine::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<float, VirtualMachine::String>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VirtualMachine::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VirtualMachine::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VirtualMachine::Object>()), std::runtime_error);
  ASSERT_THROW((test_type<int, VirtualMachine::String>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VirtualMachine::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VirtualMachine::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VirtualMachine::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<Scriptable*, VirtualMachine::String>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VirtualMachine::Boolean>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VirtualMachine::Float>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VirtualMachine::Integer>()), std::runtime_error);
  ASSERT_THROW((test_type<std::string, VirtualMachine::Object>()), std::runtime_error);
}
