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

  virtual void run_code(std::string script, std::string source) = 0;

  // TODO: Support inheritance
  virtual void expose_class(std::string name,
                            std::vector<Function> methods) = 0;
  virtual void expose_instance(std::string classname, std::string name,
                               Scriptable* owner) = 0;
  virtual void expose_object(std::string name,
                             std::vector<Function> obj) = 0;
  virtual void expose_function(Function func) = 0;
  virtual void expose_bool(std::string name, bool val) = 0;
  virtual void expose_int(std::string name, int val) = 0;
  virtual void expose_float(std::string name, float val) = 0;
  virtual void expose_string(std::string name, std::string val) = 0;
  virtual std::vector<std::unique_ptr<Type>> call_function(std::string name,
                                        std::vector<std::unique_ptr<Type>> args,
                                        Scriptable* obj = nullptr,
                                        bool func_relative = false) = 0;
  virtual void remove_entry(std::string name) = 0;

private:
  VirtualMachine(const VirtualMachine&) = delete;
  VirtualMachine& operator=(const VirtualMachine&) = delete;
};

#endif
