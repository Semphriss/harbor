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

#include "scripting/lua/lua_virtual_machine.hpp"

#include <iostream>
#include <sstream>

#include "util/log.hpp"

int
LuaVirtualMachine::call_lua(lua_State* vm)
{
  auto sqvm = get_vm(vm);

  if (!sqvm)
    return 0;

  std::string func_name = get_vm(vm)->m_last_function_name;

  const auto* func = sqvm->get_function_by_name(func_name);

  if (!func)
    return 0;

  // Parse arguments
  std::vector<std::unique_ptr<Type>> args;

  int nargs = static_cast<int>(func->m_arg_types.size());

  if (lua_gettop(vm) < nargs)
  {
    throw std::runtime_error("Not enough args to call '" + func_name + "'");
  }

  for (int i = 0; i < nargs; i++)
  {
    auto t = func->m_arg_types[static_cast<size_t>(i)];
    int argn = i - nargs;
    switch(t)
    {
      case Types::INTEGER:
      {
        if (lua_type(vm, argn) != LUA_TNUMBER)
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + func_name + "' couldn't parse"
                                  + " integer at position " + std::to_string(i
                                  + 1) + " (Lua reports: " + std::to_string(
                                    lua_type(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Integer>(static_cast<int>(lua_tonumber(vm, argn))));
        break;
      }

      case Types::FLOAT:
      {
        if (lua_type(vm, argn) != LUA_TNUMBER)
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + func_name + "' couldn't parse"
                                  + " float at position " + std::to_string(i
                                  + 1) + " (Lua reports: " + std::to_string(
                                    lua_type(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Float>(static_cast<float>(lua_tonumber(vm, argn))));
        break;
      }

      case Types::OBJECT:
      {
        // TODO: Implement this
        std::cerr << "Not implemented yet! " << __FILE__ << ":" << __LINE__ << std::endl;
        exit(1);
      }

      case Types::STRING:
      {
        if (lua_type(vm, argn) != LUA_TSTRING)
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + func_name + "' couldn't parse"
                                  + " string at position " + std::to_string(i
                                  + 1) + " (Lua reports: " + std::to_string(
                                    lua_type(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::String>(std::string(lua_tostring(vm, argn))));
        break;
      }
    }
  }

  // Call function
  auto ret = func->m_function(args);

  // Parse return value
  if (auto i = dynamic_cast<VirtualMachine::Integer*>(ret.get()))
  {
    lua_pushinteger(vm, static_cast<lua_Integer>(i->m_value));
    return 1;
  }
  else if (auto f = dynamic_cast<VirtualMachine::Float*>(ret.get()))
  {
    lua_pushnumber(vm, static_cast<lua_Number>(f->m_value));
    return 1;
  }
  else if (/* auto r = */ dynamic_cast<VirtualMachine::Object*>(ret.get()))
  {
    // TODO: Implement this
    std::cerr << "Not implemented yet! " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(1);
    //r->m_value->expose_object(*this);
    //return 1;
  }
  else if (auto s = dynamic_cast<VirtualMachine::String*>(ret.get()))
  {
    lua_pushstring(vm, s->m_value.c_str());
    return 1;
  }

  return 0;
}

LuaVirtualMachine*
LuaVirtualMachine::get_vm(const lua_State * vm)
{
  for (auto* lvm : s_vms)
  {
    if (lvm->m_vm == vm)
    {
      return lvm;
    }
  }

  return nullptr;
}

void
LuaVirtualMachine::clear_vms()
{
  for (auto it = s_vms.begin(); it != s_vms.end();)
  {
    if ((*it)->m_dead)
    {
      it = s_vms.erase(it);
    }
    else
    {
      it++;
    }
  }
}

void
LuaVirtualMachine::debugHook(lua_State * const ls, lua_Debug * const ar)
{
  // This will be called for both user-defined functions and C functions
  // We want only the latter
  if (ar->event == LUA_HOOKCALL)
  {
    auto vm = get_vm(ls);

    if (vm)
    {
      lua_getinfo(ls, "f", ar);
      auto e = lua_getupvalue(ls, -1, 1);
      if (e && std::string(e) != "_ENV") {
        const char* fn_name = lua_tostring(ls, -1);
        if (fn_name != nullptr)
        {
          vm->m_last_function_name = std::string(fn_name);
        }
      }
      lua_pop(ls, 3);
    }
  }
}

std::vector<LuaVirtualMachine*> LuaVirtualMachine::s_vms;

LuaVirtualMachine::LuaVirtualMachine() :
  m_vm(luaL_newstate()),
  m_last_function_name(),
  m_functions(),
  m_dead(false)
{
  if (!m_vm)
  {
    throw std::runtime_error("Could not create Lua VM");
  }

  luaL_openlibs(m_vm);
  lua_sethook(m_vm, debugHook, LUA_MASKCALL, 0);

  s_vms.push_back(this);
}

LuaVirtualMachine::~LuaVirtualMachine()
{
  m_dead = true;

  clear_vms();

  if (m_vm)
  {
    lua_close(m_vm);
  }
}

void
LuaVirtualMachine::run_code(std::string code, std::string source)
{
  if (luaL_loadbuffer(m_vm, code.c_str(), code.size(), source.c_str()))
  {
    std::string error_message(lua_tostring(m_vm, -1));
    lua_pop(m_vm, 1);
    throw std::runtime_error("Could not load Lua code from '" + source + "': "
                             + error_message);
  }

  if (lua_pcall(m_vm, 0, 0, 0))
  {
    std::string error_message(lua_tostring(m_vm, -1));
    lua_pop(m_vm, 1);
    throw std::runtime_error("Could not run Lua code from '" + source + "': "
                              + error_message);
  }
}

void
LuaVirtualMachine::expose_class(std::string name,
                                std::vector<ExposableFunction> methods)
{
#if LUA_VERSION_NUM > 501
  lua_pushglobaltable(m_vm);
#endif
  lua_createtable(m_vm, 0, static_cast<int>(methods.size()));

  for (auto m : methods)
  {
    auto m2 = m;
    m2.m_name = name + "::" + m.m_name;
    m_functions.push_back(m2);

    lua_pushstring(m_vm, (name + "::" + m.m_name).c_str());
    lua_pushcclosure(m_vm, call_lua, 1);
    lua_setfield(m_vm, -2, m.m_name.c_str());
  }

#if LUA_VERSION_NUM > 501
  lua_setfield(m_vm, -2, name.c_str());
  lua_pop(m_vm, 1);
#else
  lua_setglobal(m_vm, name.c_str());
#endif
}

void
LuaVirtualMachine::expose_instance(std::string /* classname */,
                                   std::string /* name */,
                                   Scriptable* /* owner */)
{
}

void
LuaVirtualMachine::expose_object(std::string /* name */,
                                 std::vector<ExposableFunction> /* obj */)
{
}

void
LuaVirtualMachine::expose_function(ExposableFunction func)
{
  m_functions.push_back(func);

#if LUA_VERSION_NUM > 501
  lua_pushglobaltable(m_vm);
#endif
  lua_pushstring(m_vm, func.m_name.c_str());
  lua_pushcclosure(m_vm, call_lua, 1);
#if LUA_VERSION_NUM > 501
  lua_setfield(m_vm, -2, func.m_name.c_str());
  lua_pop(m_vm, 1);
#else
  lua_setglobal(m_vm, func.m_name.c_str());
#endif
}

void
LuaVirtualMachine::expose_int(std::string name, int val)
{
#if LUA_VERSION_NUM > 501
  lua_pushglobaltable(m_vm);
#endif
  std::string varname = resolve_name(name, false);
  lua_pushinteger(m_vm, static_cast<lua_Integer>(val));
#if LUA_VERSION_NUM > 501
  lua_setfield(m_vm, -2, varname.c_str());
  lua_pop(m_vm, 1);
#else
  if (lua_type(m_vm, -2) == LUA_TNONE)
  {
    lua_setglobal(m_vm, varname.c_str());
  }
  else
  {
    lua_setfield(m_vm, -2, varname.c_str());
    lua_pop(m_vm, 1);
  }
#endif
}

void
LuaVirtualMachine::expose_float(std::string name, float val)
{
#if LUA_VERSION_NUM > 501
  lua_pushglobaltable(m_vm);
#endif
  std::string varname = resolve_name(name, false);
  lua_pushnumber(m_vm, static_cast<lua_Number>(val));
#if LUA_VERSION_NUM > 501
  lua_setfield(m_vm, -2, varname.c_str());
  lua_pop(m_vm, 1);
#else
  if (lua_type(m_vm, -2) == LUA_TNONE)
  {
    lua_setglobal(m_vm, varname.c_str());
  }
  else
  {
    lua_setfield(m_vm, -2, varname.c_str());
    lua_pop(m_vm, 1);
  }
#endif
}

void
LuaVirtualMachine::expose_string(std::string name, std::string val)
{
#if LUA_VERSION_NUM > 501
  lua_pushglobaltable(m_vm);
#endif
  std::string varname = resolve_name(name, false);
  lua_pushstring(m_vm, val.c_str());
#if LUA_VERSION_NUM > 501
  lua_setfield(m_vm, -2, varname.c_str());
  lua_pop(m_vm, 1);
#else
  if (lua_type(m_vm, -2) == LUA_TNONE)
  {
    lua_setglobal(m_vm, varname.c_str());
  }
  else
  {
    lua_setfield(m_vm, -2, varname.c_str());
    lua_pop(m_vm, 1);
  }
#endif
}

std::unique_ptr<VirtualMachine::Type>
LuaVirtualMachine::call_function(std::string func_name,
                                        std::vector<std::unique_ptr<Type>> args,
                                        Scriptable* obj)
{
  resolve_name(func_name, true);

  if (lua_type(m_vm, -1) != LUA_TFUNCTION)
  {
    throw std::runtime_error("Lua: " + func_name + " is not a function");
  }


}

void
LuaVirtualMachine::remove_entry(std::string name)
{
}

const VirtualMachine::ExposableFunction*
LuaVirtualMachine::get_function_by_name(const std::string& name) const
{
  for (const auto& f : m_functions)
  {
    if (f.m_name == name)
    {
      return &f;
    }
  }

  return nullptr;
}

std::string
LuaVirtualMachine::resolve_name(std::string name, bool resolve_last)
{
  std::string path = name;
  if (!resolve_last)
  {
    path.erase(path.find_last_of('.') + 1);
  }

  if (path.empty())
    return name;

  std::istringstream str(path);
  std::string varname;
  while (std::getline(str, varname, '.'))
  {
    if (varname.empty())
    {
      lua_pop(m_vm, 1);
      throw std::runtime_error("Can't resolve name with empty variable name");
    }

    // Lua 5.1 and earlier don't have pushroottable, so we need an alternative
    bool skip_remove = false;
    if (lua_type(m_vm, -1) == LUA_TNIL)
    {
#if LUA_VERSION_NUM > 501
      lua_pushglobaltable(m_vm);
      lua_getfield(m_vm, -1, varname.c_str());
#else
      lua_getfield(m_vm, LUA_GLOBALSINDEX, varname.c_str());
      skip_remove = true;
#endif
    }
    else
    {
      lua_getfield(m_vm, -1, varname.c_str());
    }

    if (!skip_remove)
      lua_remove(m_vm, -2);

    if (lua_type(m_vm, -1) == LUA_TNIL)
    {
      lua_pop(m_vm, 1);
      throw std::runtime_error("Couldn't get '" + varname + "' in '" + name + "'");
    }
  }

  return name.substr(name.find_last_of('.') + 1);
}
