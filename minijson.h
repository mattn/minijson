/*
 * JSON parser: C++ oriented JSON parser.
 */
#ifndef minijson_h
#define minijson_h

#if defined(_MSC_VER) && _MSC_VER <= 6000
#pragma warning(push)
#pragma warning(disable: 4786)
#endif

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <string.h>

//#define __MINIJSON_LIBERAL

namespace minijson {

typedef enum {
  unknown_type,
  null_type,
  boolean_type,
  number_type,
  string_type,
  array_type,
  object_type,
} type;

typedef enum {
  no_error,
  undefined_error,
  invalid_token_error,
  unknown_type_error,
  memory_allocation_error,
} error;

class value;

typedef bool boolean;
typedef double number;
typedef std::string string;
typedef std::map<string, value> object;
typedef std::vector<value> array;
typedef struct {} null_t;

null_t null;

class value {
private:
  type t;
  union {
    null_t n;
    boolean b;
    number d;
    string* s;
    array* a;
    object* o;
  } u;

public:
  value() : t(unknown_type), u() {}
  value(null_t n) : t(null_type), u() { u.n = n; }
  value(boolean b) : t(boolean_type), u() { u.b = b; }
  value(number d) : t(boolean_type), u() { u.d = d; }
  value(const char* s) : t(string_type), u() { u.s = new string(s); }
  value(const string& s) : t(string_type), u() { u.s = new string(s); }
  value(const array& a) : t(array_type), u() { u.a = new array(a); }
  value(const object& o) : t(object_type), u() { u.o = new object(o); }
  value(const value& v) : t(v.t), u() {
    if (t == array_type) {
      u.a = new array();
      *u.a = *v.u.a;
    } else if (t == object_type) {
      u.o = new object();
      *u.o = *v.u.o;
    } else if (t == string_type) {
      u.s = new string();
      *u.s = *v.u.s;
    } else
      u.d = v.u.d;
  }
  ~value() {
    if (t == string_type) delete this->u.s;
    if (t == array_type) delete this->u.a;
    if (t == object_type) delete this->u.o;
  }

#if defined(_MSC_VER) && _MSC_VER <= 6000
  bool is(type _t) const {
    return t == _t;
  }
#endif

  template<typename T>
  bool is() const {
    if (typeid(T) == typeid(null) && t == null_type) return true;
    if (typeid(T) == typeid(true) && t == boolean_type) return true;
    if (typeid(T) == typeid(0.0) && t == number_type) return true;
    if (typeid(T) == typeid(string()) && t == string_type) return true;
    if (typeid(T) == typeid(object()) && t == object_type) return true;
    return false;
  }

#if defined(_MSC_VER) && _MSC_VER <= 6000
  template<typename T>
  void get(T& r) const {
    if (t == array_type) r = *(T*)(void*)u.a;
    else if (t == object_type) r = *(T*)(void*)u.o;
    else if (t == string_type) r = *(T*)(void*)u.s;
    else r = *(T*)(void*)(&u.d);
  }
#endif

  template<typename T>
  T& get() const {
    if (t == array_type) return *(T*)(void*)u.a;
    if (t == object_type) return *(T*)(void*)u.o;
    if (t == string_type) return *(T*)(void*)u.s;
    return *(T*)(void*)(&u.d);
  }
  null_t& operator=(null_t& n) {
    t = null_type;
    u.n = n;
    return u.n;
  }
  boolean& operator=(boolean b) {
    t = boolean_type;
    u.b = b;
    return u.b;
  }
  number& operator=(number d) {
    t = number_type;
    u.d = d;
    return u.d;
  }
  const std::string& operator=(const char* s) {
    t = string_type;
    u.s = new string(s);
    return *u.s;
  }
  const string& operator=(const string& s) {
    t = string_type;
    u.s = new string(s);
    return *u.s;
  }
  const object& operator=(const object& o) {
    t = object_type;
    u.o = new object(o);
    return *u.o;
  }
  const array& operator=(const array& a) {
    t = array_type;
    u.a = new array(a);
    return *u.a;
  }
  const value& operator=(const value& v) {
    t = v.t;
    if (t == array_type) {
      u.a = new array(*v.u.a);
    } else if (t == object_type) {
      u.o = new object(*v.u.o);
    } else if (t == string_type) {
      u.s = new string(*v.u.s);
    } else u.d = v.u.d;
    return *this;
  }
  std::string str(const char* p) const {
    std::stringstream ss;
    ss << '"';
    while (*p) {
      if (*p == '\n') ss << "\\n";
      if (*p == '\r') ss << "\\r";
      if (*p == '\t') ss << "\\t";
      else if (strchr("\"", *p)) ss << "\\" << *p;
      else ss << *p;
      p++;
    }
    ss << '"';
    return ss.str();
  }
  std::string str() const {
    std::stringstream ss;
    if (t == unknown_type) {
      ss << "undefined";
    } else
    if (t == null_type) {
      ss << "null";
    } else
    if (t == boolean_type) {
      ss << u.b ? "true" : "false";
    } else
    if (t == number_type) {
      ss << (double) u.d;
    } else
    if (t == string_type) {
      ss << str(u.s->c_str());
    } else
    if (t == array_type) {
      array::iterator i;
      ss << "[";
#if defined(_MSC_VER) && _MSC_VER <= 6000
      array& a = array();
      get(a);
#else
      array a = get<array>();
#endif
      for (i = a.begin(); i != a.end(); i++) {
        if (i != a.begin()) ss << ", ";
        ss << i->str();
      }
      ss << "]";
    } else
    if (t == object_type) {
      object::iterator i;
      ss << "{";
#if defined(_MSC_VER) && _MSC_VER <= 6000
      object& o = object();
      get(o);
#else
      object o = get<object>();
#endif
      for (i = o.begin(); i != o.end(); i++) {
        if (i != o.begin()) ss << ", ";
        ss << str(i->first.c_str());
        ss << ": " << i->second.str();
      }
      ss << "}";
    }
    return ss.str();
  }
};

#define _MINIJSON_SKIP(i) while (strchr("\r\n \t", *i)) { i++; }

template<typename Iter>
inline error
parse_object(Iter& i, value& v) {
  object o;
  i++;
  _MINIJSON_SKIP(i);
  if (*i != '\x7d') {
    while (*i) {
      value vk, vv;
      error e = parse_string(i, vk);
      if (e != no_error) return e;
      _MINIJSON_SKIP(i);
      if (*i != ':') return invalid_token_error;
      i++;
      e = parse_any(i, vv);
      if (e != no_error) return e;
#if defined(_MSC_VER) && _MSC_VER <= 6000
      string s;
      vk.get(s);
      o[s] = vv;
#else
      o[vk.get<std::string>()] = vv;
#endif
      _MINIJSON_SKIP(i);
      if (*i == '\x7d') break;
      if (*i != ',') return invalid_token_error;
      i++;
      _MINIJSON_SKIP(i);
#ifdef __MINIJSON_LIBERAL
      if (*i == '\x7d') break;
#endif
    }
  }
  v = value(o);
  i++;
  return no_error;
}

template<typename Iter>
inline error
parse_array(Iter& i, value& v) {
  array a;
  i++;
  _MINIJSON_SKIP(i);
  if (*i != ']') {
    while (*i) {
      value va;
      error e = parse_any(i, va);
      if (e != no_error) return e;
      a.push_back(va);
      _MINIJSON_SKIP(i);
      if (*i == ']') break;
      if (*i != ',') return invalid_token_error;
      i++;
      _MINIJSON_SKIP(i);
#ifdef __MINIJSON_LIBERAL
      if (*i == '\x7d') break;
#endif
    }
  }
  v = value(a);
  i++;
  return no_error;
}

template<typename Iter>
inline error
parse_null(Iter& i, value& v) {
  Iter p = i;
  if (*i == 'n' && *(i+1) == 'u' && *(i+2) == 'l' && *(i+3) == 'l') {
    i += 4;
    v = null;
  }
  if (*i && NULL == strchr(":,\x7d]\r\n ", *i)) {
    i = p;
    return undefined_error;
  }
  return no_error;
}

template<typename Iter>
inline error
parse_boolean(Iter& i, value& v) {
  Iter p = i;
  if (*i == 't' && *(i+1) == 'r' && *(i+2) == 'u' && *(i+3) == 'e') {
    i += 4;
    v = (boolean) true;
  } else if (*i == 'f' && *(i+1) == 'a' && *(i+2) == 'l' && *(i+3) == 's' && *(i+4) == 'e') {
    i += 5;
    v = (boolean) false;
  }
  if (*i && NULL == strchr(":,\x7d]\r\n ", *i)) {
    i = p;
    return undefined_error;
  }
  return no_error;
}

template<typename Iter>
inline error
parse_number(Iter& i, value& v) {
  Iter p = i;

#define _IS_NUM(x)  ('0' <= x && x <= '9')
#define _IS_ALNUM(x)  (('0' <= x && x <= '9') || ('a' <= x && x <= 'f') || ('A' <= x && x <= 'F'))
  if (*i == '0' && *(i + 1) == 'x' && _IS_ALNUM(*(i+2))) {
    i += 3;
    while (_IS_ALNUM(*i)) i++;
    v = (number) strtod(p, NULL);
  } else {
    while (_IS_NUM(*i)) i++;
    if (*i == '.') {
      i++;
      if (!_IS_NUM(*i)) {
        i = p;
        return invalid_token_error;
      }
      while (_IS_NUM(*i)) i++;
    }
    if (*i == 'e') {
      i++;
      if (!_IS_NUM(*i)) {
        i = p;
        return invalid_token_error;
      }
      while (_IS_NUM(*i)) i++;
    }
    v = (number) strtod(p, NULL);
  }
  if (*i && NULL == strchr(":,\x7d]\r\n ", *i)) {
    i = p;
    return invalid_token_error;
  }
  return no_error;
}

template<typename Iter>
inline error
parse_string(Iter& i, value& v) {
  if (*i != '"') return invalid_token_error;

  char t = *i++;
  Iter p = i;
  std::stringstream ss;
  
  while (*i && *i != t) {
    if (*i == '\\' && *(i+1)) {
      i++;
      if (*i == 'n') ss << "\n";
      else if (*i == 'r') ss << "\r";
      else if (*i == 't') ss << "\t";
      else ss << *i;
    } else {
      ss << *i;
    }
    i++;
  }
  if (!*i) return invalid_token_error;
  v = std::string(p, i-p);
  i++;
  if (*i && NULL == strchr(":,\x7d]\r\n ", *i)) {
    i = p;
    return invalid_token_error;
  }
  return no_error;
}

template<typename Iter>
inline error
parse_any(Iter& i, value& v) {
  _MINIJSON_SKIP(i);
  if (*i == '\x7b') return parse_object(i, v);
  if (*i == '[') return parse_array(i, v);
  if (*i == 't' || *i == 'f') return parse_boolean(i, v);
  if (*i == 'n') return parse_null(i, v);
  if ('0' <= *i && *i <= '9') return parse_number(i, v);
  if (*i == '"') return parse_string(i, v);
  return invalid_token_error;
}

template<typename Iter>
inline error
parse(Iter& i, value& v) {
  error e = parse_any(i, v);
  if (e == no_error) {
    _MINIJSON_SKIP(i);
    if (*i) return invalid_token_error;
  }
  return e;
}

#undef _MINIJSON_SKIP

inline const char*
errstr(error e) {
  switch (e) {
  case no_error: return "no error";
  case undefined_error: return "undefined";
  case invalid_token_error: return "invalid token";
  case unknown_type_error: return "unknown type";
  case memory_allocation_error: return "memory allocation error";
  default: return "unknown error";
  }
}

}

#if defined(_MSC_VER) && _MSC_VER <= 6000
#pragma warning(pop)
#endif

#endif /* minijson_h */
