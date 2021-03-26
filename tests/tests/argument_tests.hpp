#include <bandit/bandit.h>

#include "benchmark.hpp"

using namespace snowhouse;
using namespace bandit;

#include <argparse/argument.hpp>

go_bandit([]{
    describe("Argument", []{
        benchmark_it("[convert]", [&]{
            using namespace argparse;

            bool ok = false;

            Argument arg("", "", "");

            arg.setValue("123");
            AssertThat(arg.get(&ok), Equals("123"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(123));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<short>(&ok), Equals(123));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<char>(&ok), Equals(123));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<unsigned>(&ok), Equals(123u));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<unsigned char>(&ok), Equals(123));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<long>(&ok), Equals(123));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<float>(&ok), Equals(123.f));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<double>(&ok), Equals(123.0));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<long double>(&ok), Equals(123.0));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<bool>(&ok), Equals(false));
            AssertThat(ok, IsFalse());

            arg.setValue("10");
            AssertThat(arg.get(&ok), Equals("10"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(10));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<short>(&ok), Equals(10));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<char>(&ok), Equals(10));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<unsigned>(&ok), Equals(10u));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<unsigned char>(&ok), Equals(10));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<long>(&ok), Equals(10));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<float>(&ok), Equals(10.f));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<double>(&ok), Equals(10.0));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<long double>(&ok), Equals(10.0));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<bool>(&ok), Equals(false));
            AssertThat(ok, IsFalse());

            arg.setValue("text");
            AssertThat(arg.get(&ok), Equals("text"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<short>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned>(&ok), Equals(0u));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<float>(&ok), Equals(0.f));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<double>(&ok), Equals(0.0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long double>(&ok), Equals(0.0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<bool>(&ok), Equals(false));
            AssertThat(ok, IsFalse());

            arg.setValue("10.5");
            AssertThat(arg.get(&ok), Equals("10.5"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<short>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned>(&ok), Equals(0u));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<float>(&ok), Equals(10.5f));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<double>(&ok), Equals(10.5));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<long double>(&ok), Equals(10.5));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<bool>(&ok), Equals(false));
            AssertThat(ok, IsFalse());

            arg.setValue("true");
            AssertThat(arg.get(&ok), Equals("true"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<short>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned>(&ok), Equals(0u));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<float>(&ok), Equals(0.f));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<double>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long double>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<bool>(&ok), Equals(true));
            AssertThat(ok, IsTrue());

            arg.setValue("false");
            AssertThat(arg.get(&ok), Equals("false"));
            AssertThat(ok, IsTrue());
            AssertThat(arg.get<int>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<short>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned>(&ok), Equals(0u));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<unsigned char>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<float>(&ok), Equals(0.f));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<double>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<long double>(&ok), Equals(0));
            AssertThat(ok, IsFalse());
            AssertThat(arg.get<bool>(&ok), Equals(false));
            AssertThat(ok, IsTrue());
        });
    });
});
