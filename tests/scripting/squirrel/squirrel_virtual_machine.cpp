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
#include "util/log.hpp"

#define private public
#define protected public
#define final

#include "scripting/squirrel/squirrel_virtual_machine.hpp"

#undef private
#undef protected
#undef final

class ScriptTest :
  public Scriptable
{
public:
  ScriptTest() = default;

  virtual std::string get_classname() const override
  {
    return "ScriptTest";
  }

  void activate()
  {
    a = true;
  }

  bool activated() const
  {
    return a;
  }

private:
  bool a = false;
};

TEST(Scripting_Squirrel_SquirrelVirtualMachine, print)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);
  vm.run_code("print(\"Hello, world!\");", "<test>");

  ASSERT_EQ(ss.str(), "Hello, world!\n");
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, on_compile_error)
{
  std::ostream* old_log = Log::s_log;
  std::stringstream ss;
  Log::s_log = &ss;

  SquirrelVirtualMachine vm;
  ASSERT_THROW(vm.run_code("local a <- \"Hello!\";\na |;", "<test>"),
               std::runtime_error);

  std::string output = ss.str();
  ASSERT_TRUE(output.length() >= 49 && 0 == output.compare(output.length() - 49,
              49, "<test>:1:10: end of statement expected (; or lf)\n"));

  Log::s_log = old_log;
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, get_vm)
{
  // TODO
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, clear_vms)
{
  ASSERT_EQ(SquirrelVirtualMachine::s_vms.size(), 0);
  SquirrelVirtualMachine vm1;
  ASSERT_EQ(SquirrelVirtualMachine::s_vms.size(), 1);
  {
    ASSERT_EQ(SquirrelVirtualMachine::s_vms.size(), 1);
    SquirrelVirtualMachine vm2;
    ASSERT_EQ(SquirrelVirtualMachine::s_vms.size(), 2);
  }
  ASSERT_EQ(SquirrelVirtualMachine::s_vms.size(), 1);
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, squirrel_call)
{
  SquirrelVirtualMachine vm;

  bool e = false;

  ASSERT_NO_THROW({
    vm.expose_function({"test", VirtualMachine::bind(std::function<void()>([&e]{
      e = true;
    }))});
  });

  ASSERT_NO_THROW({
    vm.call_function("test", {});
  });

  ASSERT_EQ(e, true);
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, ctor_dtor)
{
  SquirrelVirtualMachine vm;
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, run_code)
{
  std::ostream* old_log = Log::s_log;
  std::stringstream ss;
  Log::s_log = &ss;

  SquirrelVirtualMachine vm;

  // Taken from http://www.squirrel-lang.org/#look
  // https://web.archive.org/web/20210608224407/http://www.squirrel-lang.org/
  ASSERT_NO_THROW({
    vm.run_code(
      "local table = {"
      "  a = \"10\""
      "  subtable = {"
      "    array = [1,2,3]"
      "  },"
      "  [10 + 123] = \"expression index\""
      "}"
      ""
      "local array=[ 1, 2, 3, { a = 10, b = \"string\" } ];"
      ""
      "foreach (i,val in array)"
      "{"
      "  local xxx = (\"the type of val is\"+typeof val);"
      "}"
      ""
      "/////////////////////////////////////////////"
      ""
      "class Entity"
      "{"
      "  constructor(etype,entityname)"
      "  {"
      "    name = entityname;"
      "    type = etype;"
      "  }"
      ""
      "  x = 0;"
      "  y = 0;"
      "  z = 0;"
      "  name = null;"
      "  type = null;"
      "}"
      ""
      "function Entity::MoveTo(newx,newy,newz)"
      "{"
      "  x = newx;"
      "  y = newy;"
      "  z = newz;"
      "}"
      ""
      "class Player extends Entity {"
      "  constructor(entityname)"
      "  {"
      "    base.constructor(\"Player\",entityname)"
      "  }"
      "  function DoDomething()"
      "  {"
      "    ::print(\"something\");"
      "  }"
      ""
      "}"
      ""
      "local newplayer = Player(\"da playar\");"
      ""
      "newplayer.MoveTo(100,200,300);",
      "<long_test>"
    );
  });

  ASSERT_EQ(ss.str(), "");

  Log::s_log = old_log;
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_class)
{
  std::ostream* old_log = Log::s_log;
  std::stringstream ss;
  Log::s_log = &ss;

  SquirrelVirtualMachine vm;

  bool a = false;
  int b = 3;

  ASSERT_NO_THROW({
    vm.expose_class("MyClass", {
      {"method1", VirtualMachine::bind(std::function<void()>([&a]{
        a = true;
      }))},
      {"method2", VirtualMachine::bind(std::function<int(int)>([&b](int c){
        b = c;
        return static_cast<int>(static_cast<float>(b) * 1.7f);
      }))},
    });
  });

  ASSERT_NO_THROW({
    vm.run_code("local i = MyClass(); i.method1(); i.method2(i.method2(5))",
                "<test>");
  });

  ASSERT_EQ(a, true);
  ASSERT_EQ(b, 8);

  ASSERT_EQ(ss.str(), "");

  Log::s_log = old_log;
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_instance)
{
  std::ostream* old_log = Log::s_log;
  std::stringstream ss;
  Log::s_log = &ss;

  SquirrelVirtualMachine vm;

  ScriptTest st;

  ASSERT_NO_THROW({
    vm.expose_class("MyClass", {
      {"method1", VirtualMachine::bind(std::function<void(Scriptable*)>(
        [](Scriptable* obj){
          ASSERT_NE(obj, nullptr);
          auto* st_obj = dynamic_cast<ScriptTest*>(obj);
          ASSERT_NE(st_obj, nullptr);
          st_obj->activate();
        }
      ))}
    });
  });

  ASSERT_NO_THROW({
    vm.expose_instance("MyClass", "my_instance", &st);
  });

  ASSERT_FALSE(st.activated());

  ASSERT_NO_THROW({
    vm.run_code("my_instance.method1();", "<test>");
  });

  ASSERT_TRUE(st.activated());

  ASSERT_EQ(ss.str(), "");

  Log::s_log = old_log;
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_object)
{
  SquirrelVirtualMachine vm;

  bool a = false;

  ASSERT_NO_THROW({
    vm.expose_object("my_object", {
      {"method1", VirtualMachine::bind(std::function<void()>([&a](){
        a = true;
      }))}
    });
  });

  ASSERT_NO_THROW({
    vm.run_code("my_object.method1();", "<test>");
  });

  ASSERT_TRUE(a);
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_function)
{
  SquirrelVirtualMachine vm;

  bool a = false;

  ASSERT_NO_THROW({
    vm.expose_function({"method1", VirtualMachine::bind(std::function<void()>(
      [&a](){
        a = true;
      }
    ))});
  });

  ASSERT_NO_THROW({
    vm.run_code("method1();", "<test>");
  });

  ASSERT_TRUE(a);
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_bool)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);

  ASSERT_NO_THROW({
    vm.expose_bool("my_bool", true);
    vm.expose_bool("my_bool_2", false);
  });

  ASSERT_NO_THROW({
    vm.run_code("print(my_bool); print(my_bool_2);", "<test>");
  });

  ASSERT_EQ(ss.str(), "true\nfalse\n");
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_int)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);

  ASSERT_NO_THROW({
    vm.expose_int("my_int", 17);
    vm.expose_int("my_int_2", 2147483647);
    vm.expose_int("my_int_3", -2147483647);
  });

  ASSERT_NO_THROW({
    vm.run_code("print(my_int); print(my_int_2); print(my_int_3);", "<test>");
  });

  ASSERT_EQ(ss.str(), "17\n2147483647\n-2147483647\n");
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_float)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);

  ASSERT_NO_THROW({
    vm.expose_float("my_float", 19.375f);
    vm.expose_float("my_float_2", -1.93000e-7);
  });

  ASSERT_NO_THROW({
    vm.run_code("print(my_float); print(my_float_2);", "<test>");
  });

  ASSERT_EQ(ss.str(), "19.375\n-1.93e-07\n");
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, expose_string)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);

  ASSERT_NO_THROW({
    vm.expose_string("my_string", "Advanced string à caractères spéciaux"
                     " بما في ذلك الأحرف من اليمين إلى اليسار! 😁🎉");
  });

  ASSERT_NO_THROW({
    vm.run_code("print(my_string); print(\"ÆÜĞH! 🐡 哇\");", "<test>");
  });

  ASSERT_EQ(ss.str(), "Advanced string à caractères spéciaux"
                      " بما في ذلك الأحرف من اليمين إلى اليسار! 😁🎉\n"
                      "ÆÜĞH! 🐡 哇\n");
}

// TODO: Test every return type possible for each
//  - Squirrel function
//  - Native function
//  - Native class function
TEST(Scripting_Squirrel_SquirrelVirtualMachine, call_function)
{
  SquirrelVirtualMachine vm;
  std::stringstream ss;

  vm.set_print(&ss);

  bool a = false;
  ScriptTest st1, st2, st3;

  ASSERT_NO_THROW({
    vm.expose_function({"method0", VirtualMachine::bind(
      std::function<void()>([&a] { a = true; })
    )});
  });

  ASSERT_NO_THROW({
    vm.run_code("function method1(i, s, b, f, o) {"
                "  print(i);"
                "  print(s);"
                "  print(b);"
                "  print(f);"
                "  print(typeof o);"
                "  o.activate();"
                "  return o == st1;"
                "}"
                "potato <- this;"
                "function test_this_root() {"
                "  return potato == this;"
                "}", "<declarations>");
  });

  ASSERT_NO_THROW({
    vm.expose_function({"method2", VirtualMachine::bind(
      std::function<int(int, std::string, bool, float, Scriptable*)>(
        [&ss](int i, std::string s, bool b, float f, Scriptable* o){
          std::string oo;
          if (o)
          {
            if (auto o2 = dynamic_cast<ScriptTest*>(o))
            {
              oo = o->get_classname();
              o2->activate();
            }
          }
          ss << i << s << b << f << oo;
          return 92;
        }
      )
    )});
  });

  ASSERT_NO_THROW({
    vm.expose_class("MyClass", {
      {"method3", VirtualMachine::bind(
        std::function<Scriptable*(Scriptable*, int, std::string, bool, float)>(
          [&ss](Scriptable* o, int i, std::string s, bool b, float f){
            std::string oo;
            if (o)
            {
              if (auto o2 = dynamic_cast<ScriptTest*>(o))
              {
                oo = o->get_classname();
                o2->activate();
              }
            }
            ss << i << s << b << f << oo;
            return o;
          }
        )
      )},
      {"activate", VirtualMachine::bind(std::function<void(Scriptable*)>(
        [](Scriptable* o){
          if (o)
          {
            if (auto o2 = dynamic_cast<ScriptTest*>(o))
            {
              o2->activate();
            }
          }
        }
      ))}
    });
  });

  ASSERT_NO_THROW({ vm.expose_instance("MyClass", "st1", &st1); });
  ASSERT_NO_THROW({ vm.expose_instance("MyClass", "st2", &st2); });
  ASSERT_NO_THROW({ vm.expose_instance("MyClass", "st3", &st3); });

  ASSERT_NO_THROW({
    auto r0 = vm.call_function("method0", {});
    ASSERT_EQ(r0.size(), 0);
  });

  ASSERT_NO_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Integer>(2));
    args.push_back(std::make_unique<VirtualMachine::String>("Hello"));
    args.push_back(std::make_unique<VirtualMachine::Boolean>(true));
    args.push_back(std::make_unique<VirtualMachine::Float>(5.6f));
    args.push_back(std::make_unique<VirtualMachine::Object>(&st1));
    auto r1 = vm.call_function("method1", std::move(args));

    ASSERT_EQ(r1.size(), 1);
    auto val = dynamic_cast<VirtualMachine::Boolean*>(r1.back().get());
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(val->m_value, true);
  });

  ASSERT_NO_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Integer>(7));
    args.push_back(std::make_unique<VirtualMachine::String>("World!"));
    args.push_back(std::make_unique<VirtualMachine::Boolean>(false));
    args.push_back(std::make_unique<VirtualMachine::Float>(7.8f));
    args.push_back(std::make_unique<VirtualMachine::Object>(&st2));
    auto r2 = vm.call_function("method2", std::move(args));

    ASSERT_EQ(r2.size(), 1);
    auto val = dynamic_cast<VirtualMachine::Integer*>(r2.back().get());
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(val->m_value, 92);
  });

  ASSERT_NO_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Integer>(176));
    args.push_back(std::make_unique<VirtualMachine::String>("=)"));
    args.push_back(std::make_unique<VirtualMachine::Boolean>(true));
    args.push_back(std::make_unique<VirtualMachine::Float>(72.25f));
    auto r3 = vm.call_function("method3", std::move(args), &st3, true);

    ASSERT_EQ(r3.size(), 1);
    auto val = dynamic_cast<VirtualMachine::Object*>(r3.back().get());
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(val->m_value, &st3);
  });

  EXPECT_TRUE(a);
  EXPECT_TRUE(st1.activated());
  EXPECT_TRUE(st2.activated());
  EXPECT_TRUE(st3.activated());

  EXPECT_EQ(ss.str(), "2\nHello\ntrue\n5.6\ninstance\n7World!07.8ScriptTest176"
                      "=)172.25ScriptTest");
}

TEST(Scripting_Squirrel_SquirrelVirtualMachine, push_instance)
{
  SquirrelVirtualMachine vm;

  ScriptTest st1, st2;

  ASSERT_NO_THROW({
    vm.expose_function({"method_no_st", VirtualMachine::bind(
      std::function<void(Scriptable*)>([](Scriptable* s){
        if (s)
        {
          if (auto* s2 = dynamic_cast<ScriptTest*>(s))
          {
            s2->activate();
          }
        }
      })
    )});
  });

  // No class 'ScriptTest' in VM
  ASSERT_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Object>(&st1));
    vm.call_function("method_no_st", std::move(args));
  }, std::runtime_error);

  // 'ScriptTest' is not a classname
  vm.expose_bool("ScriptTest", true);
  ASSERT_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Object>(&st1));
    vm.call_function("method_no_st", std::move(args));
  }, std::runtime_error);
  vm.remove_entry("ScriptTest");

  ASSERT_NO_THROW({
    vm.expose_class("ScriptTest", {
      {"activate", VirtualMachine::bind(std::function<void(Scriptable*)>(
        [](Scriptable* o){
          if (o)
          {
            if (auto o2 = dynamic_cast<ScriptTest*>(o))
            {
              o2->activate();
            }
          }
        }
      ))}
    });
  });

  ASSERT_NO_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Object>(&st1));
    vm.call_function("method_no_st", std::move(args));
  });

  ASSERT_NO_THROW({
    vm.expose_instance("ScriptTest", "st12345", &st2);
  });

  ASSERT_NO_THROW({
    std::vector<std::unique_ptr<VirtualMachine::Type>> args;
    args.push_back(std::make_unique<VirtualMachine::Object>(&st2));
    vm.call_function("method_no_st", std::move(args));
  });
}