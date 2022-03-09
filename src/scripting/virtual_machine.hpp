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

#ifndef _HEADER_HARBOR_SCRIPTING_VIRTUALMACHINE_HPP
#define _HEADER_HARBOR_SCRIPTING_VIRTUALMACHINE_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "scripting/catastrophic_failure.hpp"

class Scriptable;

/**
 * Class that represents a virtual machine for scripting.
 */
class VirtualMachine :
  protected CatastrophicFailure
{
public:
  VirtualMachine() = default;
  virtual ~VirtualMachine() = default;

  virtual void run_code(const std::string& script, const std::string& source) = 0;

  // TODO: Support inheritance
  virtual void expose_class(const std::string& name,
                            const std::vector<Function>& methods) = 0;
  virtual void expose_instance(const std::string& classname,
                               const std::string& name, Scriptable* owner) = 0;
  virtual void expose_object(const std::string& name,
                             const std::vector<Function>& obj) = 0;
  virtual void expose_function(const Function& func) = 0;
  virtual void expose_bool(const std::string& name, bool val) = 0;
  virtual void expose_int(const std::string& name, int val) = 0;
  virtual void expose_float(const std::string& name, float val) = 0;
  virtual void expose_string(const std::string& name, const std::string& val) = 0;
  virtual FnReturn call_function(const std::string& name, const FnArgs& args,
                                 Scriptable* obj = nullptr,
                                 bool func_relative = false) = 0;
  virtual void remove_entry(const std::string& name) = 0;

private:
  VirtualMachine(const VirtualMachine&) = delete;
  VirtualMachine& operator=(const VirtualMachine&) = delete;
};

#endif
