//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2021-2022 Semphris <semphris@protonmail.com>
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

#ifndef _HEADER_HARBOR_SCRIPTING_CATASTROPHICFAILURE_HPP
#define _HEADER_HARBOR_SCRIPTING_CATASTROPHICFAILURE_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Scriptable;

/**
 * Utilities for virtual machines.
 *
 * The class is not marked `final` as allowing the VirtualMachine class to
 * inherit from it helps avoid typing this class' name at long. This class is
 * (and should remain) empty.
 *
 * As implied, this file (and its corresponding body) is a catastrophic failure.
 */
class CatastrophicFailure
{
  friend class VirtualMachine;

public:
  // Forward declarations, because they will be public
  class Type;
  enum class Types;
  typedef std::vector<std::unique_ptr<Type>> FnReturn;
  typedef std::vector<std::unique_ptr<Type>> FnArgs;

private:
  // ========================================================================
  // SINGLE_ARG

  template<typename T, size_t pos, typename... Args>
  class SingleArg;

  template<size_t Pos, typename... Args>
  class SingleArg<bool, Pos, Args...>
  {
  public:
    static void parse(std::tuple<Args...>& tuple, Type* arg);
  };

  template<size_t Pos, typename... Args>
  class SingleArg<float, Pos, Args...>
  {
  public:
    static void parse(std::tuple<Args...>& tuple, Type* arg);
  };

  template<size_t Pos, typename... Args>
  class SingleArg<int, Pos, Args...>
  {
  public:
    static void parse(std::tuple<Args...>& tuple, Type* arg);
  };

  template<size_t Pos, typename... Args>
  class SingleArg<Scriptable*, Pos, Args...>
  {
  public:
    static void parse(std::tuple<Args...>& tuple, Type* arg);
  };

  template<size_t Pos, typename... Args>
  class SingleArg<std::string, Pos, Args...>
  {
  public:
    static void parse(std::tuple<Args...>& tuple, Type* arg);
  };

  // ========================================================================
  // RETURN_VAL

  template<typename T, typename... A>
  class ReturnVal;

  template<typename... A>
  class ReturnVal<void, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<void(A...)> func, const std::tuple<A...>& args);
  };

  template<typename... A>
  class ReturnVal<bool, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<bool(A...)> func, const std::tuple<A...>& args);
  };

  template<typename... A>
  class ReturnVal<float, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<float(A...)> func, const std::tuple<A...>& args);
  };

  template<typename... A>
  class ReturnVal<int, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<int(A...)> func, const std::tuple<A...>& args);
  };

  template<typename... A>
  class ReturnVal<Scriptable*, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<Scriptable*(A...)> func, const std::tuple<A...>& args);
  };

  template<typename... A>
  class ReturnVal<std::string, A...>
  {
  public:
    static std::unique_ptr<Type> exec(std::function<std::string(A...)> func, const std::tuple<A...>& args);
  };

  // ========================================================================
  // FETCH_ARG

  template<std::size_t Current, std::size_t End, typename... Args>
  class FetchArg;

  template<std::size_t End, typename... Args>
  class FetchArg<End, End, Args...>
  {
  public:
    FetchArg(std::tuple<Args...>& /* tuple */, const std::vector<std::unique_ptr<Type>>& /* args */) {}
  };

  template<std::size_t Current, std::size_t End, typename... Args>
  class FetchArg
  {
  public:
    FetchArg(std::tuple<Args...>& tuple, const std::vector<std::unique_ptr<Type>>& args);
  };

  // ========================================================================
  // LIST_ARG

  template<std::size_t Current, std::size_t End, typename... Args>
  class ListArg;

  template<std::size_t End, typename... Args>
  class ListArg<End, End, Args...>
  {
  public:
    static void list(std::vector<Types>& /* list */) {}
  };

  template<std::size_t Current, std::size_t End, typename... Args>
  class ListArg
  {
  public:
    static void list(std::vector<Types>& list);
  };

public:
  enum class Types
  {
    BOOLEAN,
    INTEGER,
    FLOAT,
    OBJECT,
    STRING
  };

  class Type
  {
    // This is just to make this class look polymorphic to the compiler
  public:
    virtual ~Type() = default;
  };

  class Boolean final :
    public Type
  {
  public:
    Boolean() = default;
    Boolean(bool v) : m_value(v) {}
  public:
    bool m_value;
  };

  class Integer final :
    public Type
  {
  public:
    Integer() = default;
    Integer(int v) : m_value(v) {}
  public:
    int m_value;
  };

  class Float final :
    public Type
  {
  public:
    Float() = default;
    Float(float v) : m_value(v) {}
  public:
    float m_value;
  };

  class Object final :
    public Type
  {
  public:
    Object() = default;
    Object(Scriptable* v) : m_value(v) {}
  public:
    Scriptable* m_value;
  };

  class String final :
    public Type
  {
  public:
    String() = default;
    String(std::string v) : m_value(v) {}
  public:
    std::string m_value;
  };

  typedef std::function<std::unique_ptr<Type>(const std::vector<std::unique_ptr<Type>>&)> ScriptFunction;

  struct Function final
  {
    std::string m_name;
    ScriptFunction m_function;
    std::vector<Types> m_arg_types;
  };

public:
  CatastrophicFailure() = default;
  virtual ~CatastrophicFailure() = default;

  template<typename R, typename... A, size_t NArgs = std::tuple_size<std::tuple<A...>>::value>
  static std::pair<ScriptFunction, std::vector<Types>> bind(std::function<R(A...)> func);

private:
  CatastrophicFailure(const CatastrophicFailure&) = delete;
  CatastrophicFailure& operator=(const CatastrophicFailure&) = delete;
};

#include "scripting/catastrophic_failure.cpp"

#endif
