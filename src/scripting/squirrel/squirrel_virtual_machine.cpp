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

#include "scripting/squirrel/squirrel_virtual_machine.hpp"

#include <stdarg.h>
#include <sstream>

#include "scripting/scriptable.hpp"
#include "util/log.hpp"

SquirrelVirtualMachine* 
SquirrelVirtualMachine::get_vm(const HSQUIRRELVM vm)
{
  for (auto* sqvm : s_vms)
  {
    if (sqvm->m_vm == vm)
    {
      return sqvm;
    }
  }

  return nullptr;
}

void
SquirrelVirtualMachine::clear_vms()
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

SQInteger
SquirrelVirtualMachine::squirrel_call(HSQUIRRELVM vm)
{
  auto sqvm = get_vm(vm);

  if (!sqvm)
    return 0;

  const SQChar* func_name;
  sq_getclosurename(vm, 0);
  sq_getstring(vm, -1, &func_name);
  sq_pop(vm, 1);

  const auto* func = sqvm->get_function_by_name(std::string(func_name));

  if (!func)
    return 0;

  // Parse arguments
  std::vector<std::unique_ptr<Type>> args;

  int nargs = static_cast<int>(func->m_arg_types.size());

  if (sq_gettop(vm) < nargs)
  {
    throw std::runtime_error("Not enough args to call '"
                              + std::string(func_name) + "'");
  }

  for (int i = 0; i < nargs; i++)
  {
    auto t = func->m_arg_types[static_cast<size_t>(i)];
    int argn = i - nargs;
    switch(t)
    {
      case Types::BOOLEAN:
      {
        SQBool bool_sq;
        if (SQ_FAILED(sq_getbool(vm, argn, &bool_sq)))
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + std::string(func_name)
                                  + "' couldn't parse boolean at position "
                                  + std::to_string(i + 1) + " (Squirrel "
                                  + "reports: " + std::to_string(
                                    sq_gettype(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Boolean>(static_cast<bool>(bool_sq)));
        break;
      }

      case Types::INTEGER:
      {
        SQInteger int_sq;
        if (SQ_FAILED(sq_getinteger(vm, argn, &int_sq)))
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + std::string(func_name)
                                  + "' couldn't parse integer at position "
                                  + std::to_string(i + 1) + " (Squirrel "
                                  + "reports: " + std::to_string(
                                    sq_gettype(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Integer>(static_cast<int>(int_sq)));
        break;
      }

      case Types::FLOAT:
      {
        SQFloat float_sq;
        if (SQ_FAILED(sq_getfloat(vm, argn, &float_sq)))
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + std::string(func_name)
                                  + "' couldn't parse float at position "
                                  + std::to_string(i + 1) + " (Squirrel "
                                  + "reports: " + std::to_string(
                                    sq_gettype(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Float>(static_cast<float>(float_sq)));
        break;
      }

      case Types::OBJECT:
      {
        SQUserPointer obj;
        if (SQ_FAILED(sq_getinstanceup(vm, argn, &obj, nullptr)))
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + std::string(func_name)
                                  + "' couldn't parse class at position "
                                  + std::to_string(i + 1) + " (Squirrel "
                                  + "reports: " + std::to_string(
                                    sq_gettype(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::Object>(static_cast<Scriptable*>(obj)));
        break;
      }

      case Types::STRING:
      {
        const char* str_sq;
        if (SQ_FAILED(sq_getstring(vm, argn, &str_sq)))
        {
          // FIXME: Print the actual type names
          throw std::runtime_error("Call to '" + std::string(func_name)
                                  + "' couldn't parse string at position "
                                  + std::to_string(i + 1) + " (Squirrel "
                                  + "reports: " + std::to_string(
                                    sq_gettype(vm, argn)) + ")");
        }
        args.push_back(std::make_unique<VirtualMachine::String>(std::string(str_sq)));
        break;
      }
    }
  }

  // Call function
  auto ret = func->m_function(args);

  // Parse return value
  if (auto b = dynamic_cast<VirtualMachine::Boolean*>(ret.get()))
  {
    sq_pushbool(vm, b->m_value);
    return 1;
  }
  else if (auto i = dynamic_cast<VirtualMachine::Integer*>(ret.get()))
  {
    sq_pushinteger(vm, i->m_value);
    return 1;
  }
  else if (auto f = dynamic_cast<VirtualMachine::Float*>(ret.get()))
  {
    sq_pushfloat(vm, f->m_value);
    return 1;
  }
  else if (auto o = dynamic_cast<VirtualMachine::Object*>(ret.get()))
  {
    sqvm->push_instance(o->m_value->get_classname(), o->m_value);
    return 1;
  }
  else if (auto s = dynamic_cast<VirtualMachine::String*>(ret.get()))
  {
    sq_pushstring(vm, s->m_value.c_str(), static_cast<SQInteger>(s->m_value.size()));
    return 1;
  }

  return 0;
}

std::vector<SquirrelVirtualMachine*> SquirrelVirtualMachine::s_vms;

void
SquirrelVirtualMachine::print(HSQUIRRELVM vm, const SQChar* str, ...)
{
  char c[2048];

  va_list vl;
  va_start(vl, str);
  vsnprintf(c, 2048, str, vl);
  va_end(vl);

  auto svm = get_vm(vm);

  std::ostream* out = nullptr;
  if (svm && svm->m_print_stream)
  {
    out = svm->m_print_stream;
  }

  if (!out)
    return;

  *out << std::string(c) << std::endl;
}

void
SquirrelVirtualMachine::on_compile_error(HSQUIRRELVM /*vm*/, const SQChar* desc,
                                         const SQChar* source, SQInteger line,
                                         SQInteger column)
{
  // TODO: Get the foreign pointer form the VM and call the compile error
  // handler on the object directly

  log_error << "[SQUIRREL] " << std::string(source) << ":"
            << std::to_string(line) << ":" << std::to_string(column)
            << ": " << std::string(desc) << std::endl;
}

SquirrelVirtualMachine::SquirrelVirtualMachine() :
  m_vm(sq_open(1024)),
  m_current_top(0),
  m_dead(false),
  m_functions(),
  m_objects(),
  m_print_stream(nullptr)
{
  s_vms.push_back(this);

  sq_setforeignptr(m_vm, this);
  sq_setcompilererrorhandler(m_vm, SquirrelVirtualMachine::on_compile_error);
  sq_setprintfunc(m_vm, SquirrelVirtualMachine::print, SquirrelVirtualMachine::print);
}

SquirrelVirtualMachine::~SquirrelVirtualMachine()
{
  m_dead = true;

  clear_vms();

  if (m_vm)
  {
    sq_close(m_vm);
  }
}

void
SquirrelVirtualMachine::run_code(std::string code, std::string source)
{
  std::istringstream s(code);

  SQRESULT r = sq_compilebuffer(m_vm, code.c_str(),
                                static_cast<SQInteger>(code.size()),
                                source.c_str(),
                                SQTrue);

  if (SQ_FAILED(r))
  {
    // get_error() will return only the message, not the line/column
    throw std::runtime_error("Could not compile Squirrel script");
  }

  m_current_top = sq_gettop(m_vm);
  sq_pushroottable(m_vm);
  r = sq_call(m_vm, 1, SQFalse, SQTrue);

  if (SQ_FAILED(r))
  {
    sq_pop(m_vm, 1);
    throw std::runtime_error("Could not run Squirrel script: " + get_error());
  }

  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_class(std::string name,
                                std::vector<Function> methods)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_newclass(m_vm, false);

  for (auto& m : methods)
  {
    sq_pushstring(m_vm, m.m_name.c_str(), -1);
    sq_newclosure(m_vm, squirrel_call, 0);
    sq_setnativeclosurename(m_vm, -1, m.m_name.c_str());
    sq_newslot(m_vm, -3, SQFalse); // Add the method

    m_functions.push_back(m);
  }

  sq_newslot(m_vm, -3, SQFalse); // Add the class

  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_instance(std::string classname, std::string name, Scriptable* owner)
{
  // The lazy way to protect the stack
  auto top = sq_gettop(m_vm);

  sq_pushroottable(m_vm);
  resolve_name(classname, true);
  // FIXME: Delete below
  /*sq_pushstring(m_vm, classname.c_str(), static_cast<SQInteger>(classname.size()));
  if (SQ_FAILED(sq_get(m_vm, -2)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("No class '" + classname + "' in VM");
  }*/

  if (sq_gettype(m_vm, -1) != OT_CLASS)
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("'" + classname + "' is not a classname");
  }

  sq_pushroottable(m_vm);
  std::string varname;
  try
  {
    varname = resolve_name(name, false);
  }
  catch (std::exception& e)
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw e;
  }
  catch(...)
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw;
  }

  sq_pushstring(m_vm, varname.c_str(), -1);

  if (SQ_FAILED(sq_createinstance(m_vm, -3)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("Could not create instance of '" + classname
                              + "': " + get_error());
  }

  if (SQ_FAILED(sq_setinstanceup(m_vm, -1, owner)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("Could not bind instance of '" + classname
                              + "' to owner: " + get_error());
  }

  HSQOBJECT obj;
  sq_resetobject(&obj);
  sq_getstackobj(m_vm, -1, &obj);

  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("Could not bind instance of '" + classname
                              + "' to root table: " + get_error());
  }

  m_objects[owner] = obj;

  // Should be 2
  sq_pop(m_vm, sq_gettop(m_vm) - top);
}

void
SquirrelVirtualMachine::expose_object(std::string name,
                                      std::vector<Function> obj)
{
  // The lazy way to protect the stack
  auto top = sq_gettop(m_vm);

  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_newtable(m_vm);

  int id = rand();
  for (const auto& func : obj)
  {
    auto func_copy = func;
    func_copy.m_name = "<" + std::to_string(id) + ">::" + func_copy.m_name;
    m_functions.push_back(func_copy);

    sq_pushstring(m_vm, func.m_name.c_str(), -1);
    sq_newclosure(m_vm, squirrel_call, 0);
    sq_setnativeclosurename(m_vm, -1, func_copy.m_name.c_str());
    sq_newslot(m_vm, -3, SQFalse);
  }

  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("Could not create anonymous object: " + get_error());
  }

  sq_pop(m_vm, sq_gettop(m_vm) - top);
}

void
SquirrelVirtualMachine::expose_function(Function func)
{
  m_functions.push_back(func);

  sq_pushroottable(m_vm);
  std::string varname = resolve_name(func.m_name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_newclosure(m_vm, squirrel_call, 0);
  if (SQ_FAILED(sq_setnativeclosurename(m_vm, -1, func.m_name.c_str())))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind function name: " + get_error());
  }
  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind function to object: " + get_error());
  }
  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_bool(std::string name, bool val)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_pushbool(m_vm, static_cast<SQBool>(val));
  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind boolean to object: " + get_error());
  }
  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_int(std::string name, int val)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_pushinteger(m_vm, static_cast<SQInteger>(val));
  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind integer to object: " + get_error());
  }
  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_float(std::string name, float val)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_pushfloat(m_vm, static_cast<SQFloat>(val));
  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind float to object: " + get_error());
  }
  sq_pop(m_vm, 1);
}

void
SquirrelVirtualMachine::expose_string(std::string name, std::string val)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  sq_pushstring(m_vm, val.c_str(), -1);
  if (SQ_FAILED(sq_newslot(m_vm, -3, SQFalse)))
  {
    sq_pop(m_vm, 3);
    throw std::runtime_error("Couldn't bind string to object: " + get_error());
  }
  sq_pop(m_vm, 1);
}

/**
 * Calls a function in the squirrel environment
 * 
 * @param func_name     Path to the function, starting from root if @p obj is
 *                      null of @p func_relative is false, starting from the
 *                      object otherwise.
 * @param args          A list of arguments to pass to the function.
 * @param obj           The object that will correcpond in-code to `this` or
 *                      equivalent, if the language supports it. May be null, in
 *                      which case `this` will be either the root table if
 *                      @p func_relative is `true` or the object that contains
 *                      the function if @p func_relative is `false`.
 * @param func_relative If `true` and @p obj is non-null, the path given in
 *                      @p func_name is relative to @p obj. If `false` and
 *                      @p obj is non-null, the path given in @p func_name is
 *                      relative to the root table. If `true` and @p obj is
 *                      null, @p obj will forcibly be set to the root table. If
 *                      `false` and @p obj is null, @p obj will forcibly be set
 *                      to the object containing the function.
 * @author Semphris <semphris@protonmail.com>
 */
std::vector<std::unique_ptr<VirtualMachine::Type>>
SquirrelVirtualMachine::call_function(std::string func_name,
                                      std::vector<std::unique_ptr<Type>> args,
                                      Scriptable* obj, bool func_relative)
{
  // The lazy way to protect the stack
  auto top = sq_gettop(m_vm);

  if (obj && func_relative)
  {
    const auto& it = m_objects.find(obj);

    if (it == m_objects.end())
    {
      throw std::runtime_error("Attempt to call function on unexposed object");
    }

    sq_pushobject(m_vm, it->second);
  }
  else
  {
    sq_pushroottable(m_vm);
  }

  resolve_name(func_name, true);

  if (sq_gettype(m_vm, -1) != SQObjectType::OT_CLOSURE &&
      sq_gettype(m_vm, -1) != SQObjectType::OT_NATIVECLOSURE)
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("'" + func_name + "' isn't a function in "
                              + (obj ? "object" : "VM"));
  }

  // Push 'this'
  if (obj)
  {
    // FIXME: Don't recalculate that
    const auto& it = m_objects.find(obj);
    sq_pushobject(m_vm, it->second);
  }
  else
  {
    sq_pushroottable(m_vm);
    if (!func_relative)
    {
      resolve_name(func_name, false);
    }
  }

  // Push args
  for (const auto& arg : args)
  {
    if (auto b = dynamic_cast<const Boolean*>(arg.get()))
    {
      sq_pushbool(m_vm, b->m_value);
    }
    else if (auto i = dynamic_cast<const Integer*>(arg.get()))
    {
      sq_pushinteger(m_vm, i->m_value);
    }
    else if (auto f = dynamic_cast<const Float*>(arg.get()))
    {
      sq_pushfloat(m_vm, f->m_value);
    }
    else if (auto o = dynamic_cast<const Object*>(arg.get()))
    {
      try
      {
        push_instance(o->m_value->get_classname(), o->m_value);
      }
      catch(std::runtime_error& e)
      {
        sq_pop(m_vm, sq_gettop(m_vm) - top);
        throw e;
      }
    }
    else if (auto s = dynamic_cast<const String*>(arg.get()))
    {
      sq_pushstring(m_vm, s->m_value.c_str(), static_cast<SQInteger>(s->m_value.size()));
    }
    else
    {
      sq_pop(m_vm, sq_gettop(m_vm) - top);
      throw std::runtime_error("Unknown argument type");
    }
  }

  // Call Squirrel function
  if (SQ_FAILED(sq_call(m_vm, args.size() + 1, SQTrue, SQTrue)))
  {
    sq_pop(m_vm, sq_gettop(m_vm) - top);
    throw std::runtime_error("Could not call function: " + get_error());
  }

  std::vector<std::unique_ptr<VirtualMachine::Type>> ret;

  // Parse return value
  switch (sq_gettype(m_vm, -1))
  {
    case SQObjectType::OT_BOOL:
    {
      SQBool sqb;
      // FIXME: Uninitialized variable if call failed
      sq_getbool(m_vm, -1, &sqb);
      ret.push_back(std::make_unique<Boolean>(static_cast<bool>(sqb)));
      break;
    }

    case SQObjectType::OT_FLOAT:
    {
      SQFloat sqf;
      // FIXME: Uninitialized variable if call failed
      sq_getfloat(m_vm, -1, &sqf);
      ret.push_back(std::make_unique<Float>(static_cast<float>(sqf)));
      break;
    }

    case SQObjectType::OT_INTEGER:
    {
      SQInteger sqi;
      // FIXME: Uninitialized variable if call failed
      sq_getinteger(m_vm, -1, &sqi);
      ret.push_back(std::make_unique<Integer>(static_cast<int>(sqi)));
      break;
    }

    case SQObjectType::OT_STRING:
    {
      const SQChar* sqs;
      // FIXME: Uninitialized variable if call failed
      sq_getstring(m_vm, -1, &sqs);
      ret.push_back(std::make_unique<String>(std::string(sqs)));
      break;
    }

    case SQObjectType::OT_INSTANCE:
    {
      SQUserPointer sqp;
      // FIXME: Uninitialized variable if call failed
      sq_getinstanceup(m_vm, -1, &sqp, nullptr);
      ret.push_back(std::make_unique<Object>(static_cast<Scriptable*>(sqp)));
      break;
    }
    
    default:
      break;
  }

  sq_pop(m_vm, sq_gettop(m_vm) - top);

  return ret;
}

void
SquirrelVirtualMachine::remove_entry(std::string name)
{
  sq_pushroottable(m_vm);
  std::string varname = resolve_name(name, false);
  sq_pushstring(m_vm, varname.c_str(), -1);
  if (SQ_FAILED(sq_deleteslot(m_vm, -2, SQFalse)))
  {
    sq_pop(m_vm, 2);
    throw std::runtime_error("Could not delete slot '" + name + "': " + get_error());
  }
  sq_pop(m_vm, 1);
}

const VirtualMachine::Function*
SquirrelVirtualMachine::get_function_by_name(const std::string& name) const
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

void
SquirrelVirtualMachine::set_print(std::ostream* out)
{
  m_print_stream = out;
}

std::string
SquirrelVirtualMachine::get_error()
{
  const char* str;

  sq_getlasterror(m_vm);

  sq_tostring(m_vm, -1);

  // sq_tostring() might have failed, double-check
  if (sq_gettype(m_vm, -1) != OT_STRING)
  {
    str = "no error info";
  }
  else
  {
    sq_getstring(m_vm, -1, &str);
  }

  sq_pop(m_vm, 1);

  return std::string(str);
}

void
SquirrelVirtualMachine::push_instance(std::string classname, Scriptable* owner)
{
  auto it = m_objects.find(owner);

  if (it != m_objects.end())
  {
    sq_pushobject(m_vm, it->second);
  }
  else
  {
    // The lazy way to protect the stack
    auto top = sq_gettop(m_vm);

    sq_pushroottable(m_vm);
    sq_pushstring(m_vm, classname.c_str(), static_cast<SQInteger>(classname.size()));
    if (SQ_FAILED(sq_get(m_vm, -2)))
    {
      sq_pop(m_vm, sq_gettop(m_vm) - top);
      throw std::runtime_error("No class '" + classname + "' in VM");
    }

    if (sq_gettype(m_vm, -1) != OT_CLASS)
    {
      sq_pop(m_vm, sq_gettop(m_vm) - top);
      throw std::runtime_error("'" + classname + "' is not a classname");
    }

    if (SQ_FAILED(sq_createinstance(m_vm, -1)))
    {
      sq_pop(m_vm, sq_gettop(m_vm) - top);
      throw std::runtime_error("Could not create instance of '" + classname
                                + "': " + get_error());
    }

    if (SQ_FAILED(sq_setinstanceup(m_vm, -1, owner)))
    {
      sq_pop(m_vm, sq_gettop(m_vm) - top);
      throw std::runtime_error("Could not bind instance of '" + classname
                                + "' to owner: " + get_error());
    }

    HSQOBJECT obj;
    sq_resetobject(&obj);
    sq_getstackobj(m_vm, -1, &obj);
    m_objects[owner] = obj;

    while (sq_gettop(m_vm) - top > 1)
      sq_remove(m_vm, -2);
  }
}

std::string
SquirrelVirtualMachine::resolve_name(std::string name, bool resolve_last)
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
      sq_pop(m_vm, 1);
      throw std::runtime_error("Can't resolve name with empty variable name");
    }

    sq_pushstring(m_vm, varname.c_str(), -1);

    if (SQ_FAILED(sq_get(m_vm, -2)))
    {
      sq_pop(m_vm, 1);
      throw std::runtime_error("Couldn't get '" + varname + "' in '" + name + "'");
    }

    sq_remove(m_vm, -2);
  }

  return name.substr(name.find_last_of('.') + 1);
}
