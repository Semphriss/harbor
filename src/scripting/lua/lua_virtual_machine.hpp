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

#ifndef _HEADER_HARBOR_SCRIPTING_LUA_LUAVIRTUALMACHINE_HPP
#define _HEADER_HARBOR_SCRIPTING_LUA_LUAVIRTUALMACHINE_HPP

#include "scripting/virtual_machine.hpp"

#include <memory>

//#include <lua.hpp>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

/**
 * A Lua virtual machine.
 */
class LuaVirtualMachine final :
  public VirtualMachine
{
public:
  static int call_lua(lua_State* vm);
  static LuaVirtualMachine* get_vm(const lua_State * vm);
  static void clear_vms();
  static void debugHook(lua_State * const ls, lua_Debug * const ar);

private:
  static std::vector<LuaVirtualMachine*> s_vms;

public:
  LuaVirtualMachine();
  virtual ~LuaVirtualMachine() override;

  virtual void run_code(std::string script, std::string source) override;
  virtual void expose_class(std::string name,
                            std::vector<Function> methods) override;
  virtual void expose_instance(std::string classname, std::string name,
                               Scriptable* owner) override;
  virtual void expose_object(std::string name,
                             std::vector<Function> obj) override;
  virtual void expose_function(Function func) override;
  virtual void expose_bool(std::string name, bool val) override;
  virtual void expose_int(std::string name, int val) override;
  virtual void expose_float(std::string name, float val) override;
  virtual void expose_string(std::string name, std::string val) override;
  virtual std::vector<std::unique_ptr<Type>> call_function(std::string name,
                                        std::vector<std::unique_ptr<Type>> args,
                                        Scriptable* obj = nullptr,
                                        bool func_relative = false) override;
  virtual void remove_entry(std::string name) override;

  const Function* get_function_by_name(const std::string& name) const;

private:
  /**
   * Resolves a name, e. g. `var1.var2.name123`. Expects a root table/object to
   * start from. It will pop the table, regardless of whether or not it throws.
   * If it doesn't throw, it will have pushed the object `name123` on the stack
   * if @p resolve_last is `false`; if it is `true`, it will have pushed `var2`
   * instead.
   * 
   * @returns the name of the last object as a string, for example `"var2"`.
   * @throws if the path is incorrectly formatted or the object doesn't exist.
   */
  std::string resolve_name(std::string name, bool resolve_last = true);

private:
  lua_State* m_vm;
  std::string m_last_function_name;
  std::vector<Function> m_functions;
  bool m_dead;

private:
  LuaVirtualMachine(const LuaVirtualMachine&) = delete;
  LuaVirtualMachine& operator=(const LuaVirtualMachine&) = delete;
};

#endif
