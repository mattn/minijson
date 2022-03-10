#include <iostream>
#include <algorithm>
#include "minijson.h"
using namespace minijson;

int
main(int /*argc*/, char* /*argv*/[]) {
#ifdef __MINIJSON_LIBERAL
  const char* foo = R"(
{'empty':{"":[]}, "foo":
3.2, "bar": "baz", "baz": true,"hoge":{"a":"あい\nうお\2え"}, "moge": null, "zz": 0xff
, "rrr":[[2,"3"], "4"], "fuba": 1e9,}
)";
#else
  const char* foo = R"(
{"empty":{"":[]}, "foo":
3.2, "bar": "baz", "baz": true,"hoge":{"a":"あい\nうお\2え"}, "moge": null, "zz": 0xff
, "rrr":[[2,"3"], "4"], "fuba": 1e9}
)";
#endif
  const char* p = foo;
  value v;
  error e = parse(p, v);
  if (e != no_error) {
    std::cerr << errstr(e) << "(" << (int) (p - foo) << "):" << p << std::endl;
    return 1;
  }
  while (*p && strchr("\r\n \t", *p)) { p++; }
  if (*p) {
    std::cerr << "invalid token: " << p << std::endl;
    return 1;
  }

#if defined(_MSC_VER) && _MSC_VER <= 6000
  object& o = object();
  v.get(o);
#else
  object& o = v.get<object>();
#endif
  o["momo"] = "3.0";
#if defined(_MSC_VER) && _MSC_VER <= 6000
  array& a = array();
  o["rrr"].get(a);
#else
  array& a = o["rrr"].get<array>();
#endif
  a.push_back("3.0");
  a.push_back(null);
  o.erase(o.find("foo"));
  std::cout << v.str() << std::endl;
  return 0;
}
