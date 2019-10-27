// Formatting library for C++ - the core API
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#pragma once

#include <cassert> // TODO: mlarouche replace with our own assert
#include <cstdio>  // std::FILE
#include <iterator>
#include <type_traits>

#include "Core/Containers/String.h"
#include "Core/Containers/StringView.h"

#ifdef __has_feature
# define FMT_HAS_FEATURE(x) __has_feature(x)
#else
# define FMT_HAS_FEATURE(x) 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
# define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
# define FMT_GCC_VERSION 0
#endif

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
# define FMT_HAS_GXX_CXX11 FMT_GCC_VERSION
#else
# define FMT_HAS_GXX_CXX11 0
#endif

#ifdef _MSC_VER
# define FMT_MSC_VER _MSC_VER
#else
# define FMT_MSC_VER 0
#endif

// Define FMT_USE_NOEXCEPT to make fmt use noexcept (C++11 feature).
#ifndef FMT_USE_NOEXCEPT
# define FMT_USE_NOEXCEPT 0
#endif

#if FMT_USE_NOEXCEPT || FMT_HAS_FEATURE(cxx_noexcept) || \
    (FMT_GCC_VERSION >= 408 && FMT_HAS_GXX_CXX11) || FMT_MSC_VER >= 1900
# define FMT_DETECTED_NOEXCEPT noexcept
# define FMT_HAS_CXX11_NOEXCEPT 1
#else
# define FMT_DETECTED_NOEXCEPT throw()
# define FMT_HAS_CXX11_NOEXCEPT 0
#endif

#ifndef FMT_NOEXCEPT
# if FMT_EXCEPTIONS || FMT_HAS_CXX11_NOEXCEPT
#  define FMT_NOEXCEPT FMT_DETECTED_NOEXCEPT
# else
#  define FMT_NOEXCEPT
# endif
#endif

#ifndef FMT_BEGIN_NAMESPACE
# define FMT_BEGIN_NAMESPACE namespace Baroque::Formatting {
# define FMT_END_NAMESPACE }
#endif

#if !defined(FMT_HEADER_ONLY) && defined(_WIN32)
# ifdef FMT_EXPORT
#  define FMT_API __declspec(dllexport)
# elif defined(FMT_SHARED)
#  define FMT_API __declspec(dllimport)
# endif
#endif
#ifndef FMT_API
# define FMT_API
#endif

#ifndef FMT_ASSERT
# define FMT_ASSERT(condition, message) assert((condition) && message)
#endif

// std::result_of is defined in <functional> in gcc 4.4.
#if FMT_GCC_VERSION && FMT_GCC_VERSION <= 404
# include <functional>
#endif

FMT_BEGIN_NAMESPACE
namespace internal {

// An implementation of declval for pre-C++11 compilers such as gcc 4.
template <typename T>
typename std::add_rvalue_reference<T>::type declval() FMT_NOEXCEPT;

template <typename>
struct result_of;

template <typename F, typename... Args>
struct result_of<F(Args...)> {
  // A workaround for gcc 4.4 that doesn't allow F to be a reference.
  typedef typename std::result_of<
    typename std::remove_reference<F>::type(Args...)>::type type;
};

// Casts nonnegative integer to unsigned.
template <typename Int>
constexpr typename std::make_unsigned<Int>::type to_unsigned(Int value) {
  FMT_ASSERT(value >= 0, "negative value");
  return static_cast<typename std::make_unsigned<Int>::type>(value);
}

/** A contiguous memory buffer with an optional growing ability. */
template <typename T>
class basic_buffer {
 private:
  basic_buffer(const basic_buffer &) = delete;
  void operator=(const basic_buffer &) = delete;

  T *ptr_;
  std::size_t size_;
  std::size_t capacity_;

 protected:
  // Don't initialize ptr_ since it is not accessed to save a few cycles.
  basic_buffer(std::size_t sz) FMT_NOEXCEPT: size_(sz), capacity_(sz) {}

  basic_buffer(T *p = nullptr, std::size_t sz = 0, std::size_t cap = 0)
    FMT_NOEXCEPT: ptr_(p), size_(sz), capacity_(cap) {}

  /** Sets the buffer data and capacity. */
  void set(T *buf_data, std::size_t buf_capacity) FMT_NOEXCEPT {
    ptr_ = buf_data;
    capacity_ = buf_capacity;
  }

  /** Increases the buffer capacity to hold at least *capacity* elements. */
  virtual void grow(std::size_t capacity) = 0;

 public:
  typedef T value_type;
  typedef const T &const_reference;

  virtual ~basic_buffer() {}

  T *begin() FMT_NOEXCEPT { return ptr_; }
  T *end() FMT_NOEXCEPT { return ptr_ + size_; }

  /** Returns the size of this buffer. */
  std::size_t size() const FMT_NOEXCEPT { return size_; }

  /** Returns the capacity of this buffer. */
  std::size_t capacity() const FMT_NOEXCEPT { return capacity_; }

  /** Returns a pointer to the buffer data. */
  T *data() FMT_NOEXCEPT { return ptr_; }

  /** Returns a pointer to the buffer data. */
  const T *data() const FMT_NOEXCEPT { return ptr_; }

  /**
    Resizes the buffer. If T is a POD type new elements may not be initialized.
   */
  void resize(std::size_t new_size) {
    reserve(new_size);
    size_ = new_size;
  }

  /** Clears this buffer. */
  void clear() { size_ = 0; }

  /** Reserves space to store at least *capacity* elements. */
  void reserve(std::size_t new_capacity) {
    if (new_capacity > capacity_)
      grow(new_capacity);
  }

  void push_back(const T &value) {
    reserve(size_ + 1);
    ptr_[size_++] = value;
  }

  /** Appends data to the end of the buffer. */
  template <typename U>
  void append(const U *begin, const U *end);

  T &operator[](std::size_t index) { return ptr_[index]; }
  const T &operator[](std::size_t index) const { return ptr_[index]; }
};

typedef basic_buffer<char> buffer;
typedef basic_buffer<wchar_t> wbuffer;

// A container-backed buffer.
template <typename Container>
class container_buffer : public basic_buffer<typename Container::Value> {
 private:
  Container &container_;

 protected:
  void grow(std::size_t capacity) override {
    container_.Resize(capacity);
    this->set(&container_[0], capacity);
  }

 public:
  explicit container_buffer(Container &c)
    : basic_buffer<typename Container::Value>(c.Size()), container_(c) {}
};

// Extracts a reference to the container from back_insert_iterator.
template <typename Container>
inline Container &get_container(std::back_insert_iterator<Container> it) {
  typedef std::back_insert_iterator<Container> bi_iterator;
  struct accessor: bi_iterator {
    accessor(bi_iterator iter) : bi_iterator(iter) {}
    using bi_iterator::container;
  };
  return *accessor(it).container;
}

struct error_handler {
  constexpr error_handler() {}
  constexpr error_handler(const error_handler &) {}

  // This function is intentionally not constexpr to give a compile-time error.
  FMT_API void on_error(const char *message);
};

template <typename T>
struct no_formatter_error : std::false_type {};
}  // namespace internal

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 405
template <typename... T>
struct is_constructible: std::false_type {};
#else
template <typename... T>
struct is_constructible : std::is_constructible<T...> {};
#endif

/**
  \rst
  The function ``ToStringView`` adapts non-intrusively any kind of string or
  string-like type if the user provides a (possibly templated) overload of
  ``ToStringView`` which takes an instance of the string class
  ``StringType<Char>`` and returns a ``fmt::basic_string_view<Char>``.
  The conversion function must live in the very same namespace as
  ``StringType<Char>`` to be picked up by ADL. Non-templated string types
  like f.e. QString must return a ``basic_string_view`` with a fixed matching
  char type.

  **Example**::

    namespace my_ns {
    inline string_view ToStringView(const my_string &s) {
      return {s.data(), s.length()};
    }
    }

    std::string message = fmt::format(my_string("The answer is {}"), 42);
  \endrst
 */
inline Baroque::StringView ToStringView(Baroque::StringView s) { return s; }

template <typename Allocator>
inline Baroque::StringView ToStringView(const Baroque::StringImplementation<Allocator> &s) { return s; }

inline Baroque::StringView ToStringView(const char *s) { return s; }

// A base class for compile-time strings. It is defined in the fmt namespace to
// make formatting functions visible via ADL, e.g. format(fmt("{}"), 42).
struct compile_string {};

template <typename S>
struct is_compile_string : std::is_base_of<compile_string, S> {};

template <
  typename S,
  typename Enable = typename std::enable_if<is_compile_string<S>::value>::type>
constexpr Baroque::StringView ToStringView(const S &s) { return s; }

template <typename Context>
class basic_format_arg;

template <typename Context>
class basic_format_args;

// A formatter for objects of type T.
template <typename T, typename Char = char, typename Enable = void>
struct formatter {
  static_assert(internal::no_formatter_error<T>::value,
    "don't know how to format the type, include fmt/ostream.h if it provides "
    "an operator<< that should be used");

  // The following functions are not defined intentionally.
  template <typename ParseContext>
  typename ParseContext::iterator parse(ParseContext &);
  template <typename FormatContext>
  auto format(const T &val, FormatContext &ctx) -> decltype(ctx.out());
};

template <typename T, typename Char, typename Enable = void>
struct convert_to_int: std::integral_constant<
  bool, !std::is_arithmetic<T>::value && std::is_convertible<T, int>::value> {};

namespace internal {

struct dummy_string_view { typedef void char_type; };
dummy_string_view ToStringView(...);
using Baroque::Formatting::ToStringView;

// Specifies whether S is a string type convertible to fmt::basic_string_view.
template <typename S>
struct is_string : std::integral_constant<bool, !std::is_same<
    dummy_string_view, decltype(ToStringView(declval<S>()))>::value> {};

template <typename S>
struct char_t {
  typedef decltype(ToStringView(declval<S>())) result;
  typedef typename result::char_type type;
};

template <typename Char>
struct named_arg_base;

template <typename T, typename Char>
struct named_arg;

enum type {
  none_type, named_arg_type,
  // Integer types should go first,
  int_type, uint_type, long_long_type, ulong_long_type, bool_type, char_type,
  last_integer_type = char_type,
  // followed by floating-point types.
  double_type, long_double_type, last_numeric_type = long_double_type,
  cstring_type, string_type, pointer_type, custom_type
};

constexpr bool is_integral(type t) {
  FMT_ASSERT(t != internal::named_arg_type, "invalid argument type");
  return t > internal::none_type && t <= internal::last_integer_type;
}

constexpr bool is_arithmetic(type t) {
  FMT_ASSERT(t != internal::named_arg_type, "invalid argument type");
  return t > internal::none_type && t <= internal::last_numeric_type;
}

template <typename Char>
struct string_value {
  const Char *value;
  std::size_t size;
};

template <typename Context>
struct custom_value {
  const void *value;
  void (*format)(const void *arg, Context &ctx);
};

// A formatting argument value.
template <typename Context>
class value {
 public:
  typedef typename Context::char_type char_type;

  union {
    int int_value;
    unsigned uint_value;
    long long long_long_value;
    unsigned long long ulong_long_value;
    double double_value;
    long double long_double_value;
    const void *pointer;
    string_value<char_type> string;
    string_value<signed char> sstring;
    string_value<unsigned char> ustring;
    custom_value<Context> custom;
  };

  constexpr value(int val = 0) : int_value(val) {}
  value(unsigned val) { uint_value = val; }
  value(long long val) { long_long_value = val; }
  value(unsigned long long val) { ulong_long_value = val; }
  value(double val) { double_value = val; }
  value(long double val) { long_double_value = val; }
  value(const char_type *val) { string.value = val; }
  value(const signed char *val) {
    static_assert(std::is_same<char, char_type>::value,
                  "incompatible string types");
    sstring.value = val;
  }
  value(const unsigned char *val) {
    static_assert(std::is_same<char, char_type>::value,
                  "incompatible string types");
    ustring.value = val;
  }
  value(Baroque::StringView val) {
    string.value = val.Data();
    string.size = val.Size();
  }
  value(const void *val) { pointer = val; }

  template <typename T>
  explicit value(const T &val) {
    custom.value = &val;
    custom.format = &format_custom_arg<T>;
  }

  const named_arg_base<char_type> &as_named_arg() {
    return *static_cast<const named_arg_base<char_type>*>(pointer);
  }

 private:
  // Formats an argument of a custom type, such as a user-defined class.
  template <typename T>
  static void format_custom_arg(const void *arg, Context &ctx) {
    // Get the formatter type through the context to allow different contexts
    // have different extension points, e.g. `formatter<T>` for `format` and
    // `printf_formatter<T>` for `printf`.
    typename Context::template formatter_type<T>::type f;
    auto &&parse_ctx = ctx.parse_context();
    parse_ctx.advance_to(f.parse(parse_ctx));
    ctx.advance_to(f.format(*static_cast<const T*>(arg), ctx));
  }
};

// Value initializer used to delay conversion to value and reduce memory churn.
template <typename Context, typename T, type TYPE>
struct init {
  T val;
  static const type type_tag = TYPE;

  constexpr init(const T &v) : val(v) {}
  constexpr operator value<Context>() const { return value<Context>(val); }
};

template <typename Context, typename T>
constexpr basic_format_arg<Context> make_arg(const T &value);

#define FMT_MAKE_VALUE(TAG, ArgType, ValueType) \
  template <typename C> \
  constexpr init<C, ValueType, TAG> make_value(ArgType val) { \
    return static_cast<ValueType>(val); \
  }

#define FMT_MAKE_VALUE_SAME(TAG, Type) \
  template <typename C> \
  constexpr init<C, Type, TAG> make_value(Type val) { return val; }

FMT_MAKE_VALUE(bool_type, bool, int)
FMT_MAKE_VALUE(int_type, short, int)
FMT_MAKE_VALUE(uint_type, unsigned short, unsigned)
FMT_MAKE_VALUE_SAME(int_type, int)
FMT_MAKE_VALUE_SAME(uint_type, unsigned)

// To minimize the number of types we need to deal with, long is translated
// either to int or to long long depending on its size.
typedef std::conditional<sizeof(long) == sizeof(int), int, long long>::type
        long_type;
FMT_MAKE_VALUE(
    (sizeof(long) == sizeof(int) ? int_type : long_long_type), long, long_type)
typedef std::conditional<sizeof(unsigned long) == sizeof(unsigned),
                         unsigned, unsigned long long>::type ulong_type;
FMT_MAKE_VALUE(
    (sizeof(unsigned long) == sizeof(unsigned) ? uint_type : ulong_long_type),
    unsigned long, ulong_type)

FMT_MAKE_VALUE_SAME(long_long_type, long long)
FMT_MAKE_VALUE_SAME(ulong_long_type, unsigned long long)
FMT_MAKE_VALUE(int_type, signed char, int)
FMT_MAKE_VALUE(uint_type, unsigned char, unsigned)

// This doesn't use FMT_MAKE_VALUE because of ambiguity in gcc 4.4.
template <typename C, typename Char>
constexpr typename std::enable_if<
  std::is_same<typename C::char_type, Char>::value,
  init<C, int, char_type>>::type make_value(Char val) { return val; }

template <typename C>
constexpr typename std::enable_if<
  !std::is_same<typename C::char_type, char>::value,
  init<C, int, char_type>>::type make_value(char val) { return val; }

FMT_MAKE_VALUE(double_type, float, double)
FMT_MAKE_VALUE_SAME(double_type, double)
FMT_MAKE_VALUE_SAME(long_double_type, long double)

// Formatting of wide strings into a narrow buffer and multibyte strings
// into a wide buffer is disallowed (https://github.com/fmtlib/fmt/pull/606).
FMT_MAKE_VALUE(cstring_type, typename C::char_type*,
               const typename C::char_type*)
FMT_MAKE_VALUE(cstring_type, const typename C::char_type*,
               const typename C::char_type*)

FMT_MAKE_VALUE(cstring_type, signed char*, const signed char*)
FMT_MAKE_VALUE_SAME(cstring_type, const signed char*)
FMT_MAKE_VALUE(cstring_type, unsigned char*, const unsigned char*)
FMT_MAKE_VALUE_SAME(cstring_type, const unsigned char*)
FMT_MAKE_VALUE_SAME(string_type, Baroque::StringView)
FMT_MAKE_VALUE(string_type, const Baroque::String&, Baroque::StringView)
FMT_MAKE_VALUE(pointer_type, void*, const void*)
FMT_MAKE_VALUE_SAME(pointer_type, const void*)
FMT_MAKE_VALUE(pointer_type, std::nullptr_t, const void*)

// Formatting of arbitrary pointers is disallowed. If you want to output a
// pointer cast it to "void *" or "const void *". In particular, this forbids
// formatting of "[const] volatile char *" which is printed as bool by
// iostreams.
template <typename C, typename T>
typename std::enable_if<!std::is_same<T, typename C::char_type>::value>::type
    make_value(const T *) {
  static_assert(!sizeof(T), "formatting of non-void pointers is disallowed");
}

template <typename C, typename T>
inline typename std::enable_if<
    std::is_enum<T>::value && convert_to_int<T, typename C::char_type>::value,
    init<C, int, int_type>>::type
  make_value(const T &val) { return static_cast<int>(val); }

template <typename C, typename T, typename Char = typename C::char_type>
inline typename std::enable_if<
    is_constructible<Baroque::StringView, T>::value &&
    !internal::is_string<T>::value,
    init<C, Baroque::StringView, string_type>>::type
  make_value(const T &val) { return Baroque::StringView(val); }

template <typename C, typename T, typename Char = typename C::char_type>
inline typename std::enable_if<
    !convert_to_int<T, Char>::value && !std::is_same<T, Char>::value &&
    !std::is_convertible<T, Baroque::StringView>::value &&
    !is_constructible<Baroque::StringView, T>::value &&
    !internal::is_string<T>::value,
    // Implicit conversion to std::string is not handled here because it's
    // unsafe: https://github.com/fmtlib/fmt/issues/729
    init<C, const T &, custom_type>>::type
  make_value(const T &val) { return val; }

template <typename C, typename T>
init<C, const void*, named_arg_type>
    make_value(const named_arg<T, typename C::char_type> &val) {
  basic_format_arg<C> arg = make_arg<C>(val.value);
  std::memcpy(val.data, &arg, sizeof(arg));
  return static_cast<const void*>(&val);
}

template <typename C, typename S>
constexpr typename std::enable_if<
  internal::is_string<S>::value,
  init<C, Baroque::StringView, string_type>>::type
    make_value(const S &val) {
  // Handle adapted strings.
  static_assert(std::is_same<
    typename C::char_type, typename internal::char_t<S>::type>::value,
    "mismatch between char-types of context and argument");
  return ToStringView(val);
}

// Maximum number of arguments with packed types.
enum { max_packed_args = 15 };
enum : unsigned long long { is_unpacked_bit = 1ull << 63 };

template <typename Context>
class arg_map;
}  // namespace internal

// A formatting argument. It is a trivially copyable/constructible type to
// allow storage in basic_memory_buffer.
template <typename Context>
class basic_format_arg {
 private:
  internal::value<Context> value_;
  internal::type type_;

  template <typename ContextType, typename T>
  friend constexpr basic_format_arg<ContextType>
    internal::make_arg(const T &value);

  template <typename Visitor, typename Ctx>
  friend constexpr typename internal::result_of<Visitor(int)>::type
    visit_format_arg(Visitor &&vis, const basic_format_arg<Ctx> &arg);

  friend class basic_format_args<Context>;
  friend class internal::arg_map<Context>;

  typedef typename Context::char_type char_type;

 public:
  class handle {
   public:
    explicit handle(internal::custom_value<Context> custom): custom_(custom) {}

    void format(Context &ctx) const { custom_.format(custom_.value, ctx); }

   private:
    internal::custom_value<Context> custom_;
  };

  constexpr basic_format_arg() : type_(internal::none_type) {}

  explicit operator bool() const FMT_NOEXCEPT {
    return type_ != internal::none_type;
  }

  internal::type type() const { return type_; }

  bool is_integral() const { return internal::is_integral(type_); }
  bool is_arithmetic() const { return internal::is_arithmetic(type_); }
};

struct monostate {};

/**
  \rst
  Visits an argument dispatching to the appropriate visit method based on
  the argument type. For example, if the argument type is ``double`` then
  ``vis(value)`` will be called with the value of type ``double``.
  \endrst
 */
template <typename Visitor, typename Context>
constexpr typename internal::result_of<Visitor(int)>::type
    visit_format_arg(Visitor &&vis, const basic_format_arg<Context> &arg) {
  typedef typename Context::char_type char_type;
  switch (arg.type_) {
  case internal::none_type:
    break;
  case internal::named_arg_type:
    FMT_ASSERT(false, "invalid argument type");
    break;
  case internal::int_type:
    return vis(arg.value_.int_value);
  case internal::uint_type:
    return vis(arg.value_.uint_value);
  case internal::long_long_type:
    return vis(arg.value_.long_long_value);
  case internal::ulong_long_type:
    return vis(arg.value_.ulong_long_value);
  case internal::bool_type:
    return vis(arg.value_.int_value != 0);
  case internal::char_type:
    return vis(static_cast<char_type>(arg.value_.int_value));
  case internal::double_type:
    return vis(arg.value_.double_value);
  case internal::long_double_type:
    return vis(arg.value_.long_double_value);
  case internal::cstring_type:
    return vis(arg.value_.string.value);
  case internal::string_type:
    return vis(Baroque::StringView(
                 arg.value_.string.value, arg.value_.string.size));
  case internal::pointer_type:
    return vis(arg.value_.pointer);
  case internal::custom_type:
    return vis(typename basic_format_arg<Context>::handle(arg.value_.custom));
  }
  return vis(monostate());
}

// DEPRECATED!
template <typename Visitor, typename Context>
constexpr typename internal::result_of<Visitor(int)>::type
    visit(Visitor &&vis, const basic_format_arg<Context> &arg) {
  return visit_format_arg(std::forward<Visitor>(vis), arg);
}

// Parsing context consisting of a format string range being parsed and an
// argument counter for automatic indexing.
template <typename Char, typename ErrorHandler = internal::error_handler>
class basic_parse_context : private ErrorHandler {
 private:
  Baroque::StringView format_str_;
  int next_arg_id_;

 public:
  typedef Char char_type;
  typedef typename Baroque::StringView::Iterator iterator;

  explicit constexpr basic_parse_context(
      Baroque::StringView format_str, ErrorHandler eh = ErrorHandler())
    : ErrorHandler(eh), format_str_(format_str), next_arg_id_(0) {}

  // Returns an iterator to the beginning of the format string range being
  // parsed.
  constexpr iterator begin() const FMT_NOEXCEPT {
    return format_str_.begin();
  }

  // Returns an iterator past the end of the format string range being parsed.
  constexpr iterator end() const FMT_NOEXCEPT { return format_str_.end(); }

  // Advances the begin iterator to ``it``.
  constexpr void advance_to(iterator it) {
    format_str_.RemovePrefix(internal::to_unsigned(it - begin()));
  }

  // Returns the next argument index.
  constexpr unsigned next_arg_id();

  constexpr bool check_arg_id(unsigned) {
    if (next_arg_id_ > 0) {
      on_error("cannot switch from automatic to manual argument indexing");
      return false;
    }
    next_arg_id_ = -1;
    return true;
  }
  void check_arg_id(Baroque::StringView) {}

  constexpr void on_error(const char *message) {
    ErrorHandler::on_error(message);
  }

  constexpr ErrorHandler error_handler() const { return *this; }
};

typedef basic_parse_context<char> format_parse_context;
typedef basic_parse_context<wchar_t> wformat_parse_context;

// DEPRECATED!
typedef basic_parse_context<char> parse_context;
typedef basic_parse_context<wchar_t> wparse_context;

namespace internal {
// A map from argument names to their values for named arguments.
template <typename Context>
class arg_map {
 private:
  arg_map(const arg_map &) = delete;
  void operator=(const arg_map &) = delete;

  typedef typename Context::char_type char_type;

  struct entry {
    Baroque::StringView name;
    basic_format_arg<Context> arg;
  };

  entry *map_;
  unsigned size_;

  void push_back(value<Context> val) {
    const internal::named_arg_base<char_type> &named = val.as_named_arg();
    map_[size_] = entry{named.name, named.template deserialize<Context>()};
    ++size_;
  }

 public:
  arg_map() : map_(nullptr), size_(0) {}
  void init(const basic_format_args<Context> &args);
  ~arg_map() { delete [] map_; }

  basic_format_arg<Context> find(Baroque::StringView name) const {
    // The list is unsorted, so just return the first matching name.
    for (entry *it = map_, *end = map_ + size_; it != end; ++it) {
      if (it->name == name)
        return it->arg;
    }
    return {};
  }
};

// A type-erased reference to an std::locale to avoid heavy <locale> include.
class locale_ref {
 private:
  const void *locale_;  // A type-erased pointer to std::locale.
  friend class locale;

 public:
  locale_ref() : locale_(nullptr) {}

  template <typename Locale>
  explicit locale_ref(const Locale &loc);

  template <typename Locale>
  Locale get() const;
};

template <typename OutputIt, typename Context, typename Char>
class context_base {
 public:
  typedef OutputIt iterator;

 private:
  basic_parse_context<Char> parse_context_;
  iterator out_;
  basic_format_args<Context> args_;
  locale_ref loc_;

 protected:
  typedef Char char_type;
  typedef basic_format_arg<Context> format_arg;

  context_base(OutputIt out, Baroque::StringView format_str,
               basic_format_args<Context> ctx_args,
               locale_ref loc = locale_ref())
  : parse_context_(format_str), out_(out), args_(ctx_args), loc_(loc) {}

  // Returns the argument with specified index.
  format_arg do_get_arg(unsigned arg_id) {
    format_arg arg = args_.get(arg_id);
    if (!arg)
      parse_context_.on_error("argument index out of range");
    return arg;
  }

  // Checks if manual indexing is used and returns the argument with
  // specified index.
  format_arg get_arg(unsigned arg_id) {
    return this->parse_context().check_arg_id(arg_id) ?
      this->do_get_arg(arg_id) : format_arg();
  }

 public:
  basic_parse_context<char_type> &parse_context() { return parse_context_; }
  basic_format_args<Context> args() const { return args_; } // DEPRECATED!
  basic_format_arg<Context> arg(unsigned id) const { return args_.get(id); }

  internal::error_handler error_handler() {
    return parse_context_.error_handler();
  }

  void on_error(const char *message) { parse_context_.on_error(message); }

  // Returns an iterator to the beginning of the output range.
  iterator out() { return out_; }
  iterator begin() { return out_; }  // deprecated

  // Advances the begin iterator to ``it``.
  void advance_to(iterator it) { out_ = it; }

  locale_ref locale() { return loc_; }
};

template <typename Context, typename T>
struct get_type {
  typedef decltype(make_value<Context>(
        declval<typename std::decay<T>::type&>())) value_type;
  static const type value = value_type::type_tag;
};

template <typename Context>
constexpr unsigned long long get_types() { return 0; }

template <typename Context, typename Arg, typename... Args>
constexpr unsigned long long get_types() {
  return get_type<Context, Arg>::value | (get_types<Context, Args...>() << 4);
}

template <typename Context, typename T>
constexpr basic_format_arg<Context> make_arg(const T &value) {
  basic_format_arg<Context> arg;
  arg.type_ = get_type<Context, T>::value;
  arg.value_ = make_value<Context>(value);
  return arg;
}

template <bool IS_PACKED, typename Context, typename T>
inline typename std::enable_if<IS_PACKED, value<Context>>::type
    make_arg(const T &value) {
  return make_value<Context>(value);
}

template <bool IS_PACKED, typename Context, typename T>
inline typename std::enable_if<!IS_PACKED, basic_format_arg<Context>>::type
    make_arg(const T &value) {
  return make_arg<Context>(value);
}
}  // namespace internal

// Formatting context.
template <typename OutputIt, typename Char>
class basic_format_context :
  public internal::context_base<
    OutputIt, basic_format_context<OutputIt, Char>, Char> {
 public:
  /** The character type for the output. */
  typedef Char char_type;

  // using formatter_type = formatter<T, char_type>;
  template <typename T>
  struct formatter_type { typedef formatter<T, char_type> type; };

 private:
  internal::arg_map<basic_format_context> map_;

  basic_format_context(const basic_format_context &) = delete;
  void operator=(const basic_format_context &) = delete;

  typedef internal::context_base<OutputIt, basic_format_context, Char> base;
  typedef typename base::format_arg format_arg;
  using base::get_arg;

 public:
  using typename base::iterator;

  /**
   Constructs a ``basic_format_context`` object. References to the arguments are
   stored in the object so make sure they have appropriate lifetimes.
   */
  basic_format_context(OutputIt out, Baroque::StringView format_str,
                       basic_format_args<basic_format_context> ctx_args,
                       internal::locale_ref loc = internal::locale_ref())
    : base(out, format_str, ctx_args, loc) {}

  format_arg next_arg() {
    return this->do_get_arg(this->parse_context().next_arg_id());
  }
  format_arg get_arg(unsigned arg_id) { return this->do_get_arg(arg_id); }

  // Checks if manual indexing is used and returns the argument with the
  // specified name.
  format_arg get_arg(Baroque::StringView name);
};

template <typename Char>
struct buffer_context {
  typedef basic_format_context<
    std::back_insert_iterator<internal::basic_buffer<Char>>, Char> type;
};
typedef buffer_context<char>::type format_context;
typedef buffer_context<wchar_t>::type wformat_context;

/**
  \rst
  An array of references to arguments. It can be implicitly converted into
  `~fmt::basic_format_args` for passing into type-erased formatting functions
  such as `~fmt::vformat`.
  \endrst
 */
template <typename Context, typename ...Args>
class format_arg_store {
 private:
  static const size_t NUM_ARGS = sizeof...(Args);

  // Packed is a macro on MinGW so use IS_PACKED instead.
  static const bool IS_PACKED = NUM_ARGS < internal::max_packed_args;

  typedef typename std::conditional<IS_PACKED,
    internal::value<Context>, basic_format_arg<Context>>::type value_type;

  // If the arguments are not packed, add one more element to mark the end.
  static const size_t DATA_SIZE =
          NUM_ARGS + (IS_PACKED && NUM_ARGS != 0 ? 0 : 1);
  value_type data_[DATA_SIZE];

  friend class basic_format_args<Context>;

  static constexpr unsigned long long get_types() {
    return IS_PACKED ?
      internal::get_types<Context, Args...>() :
      internal::is_unpacked_bit | NUM_ARGS;
  }

 public:
  static constexpr unsigned long long TYPES = get_types();

#if (FMT_GCC_VERSION && FMT_GCC_VERSION <= 405) || \
    (FMT_MSC_VER && FMT_MSC_VER <= 1800)
  // Workaround array initialization issues in gcc <= 4.5 and MSVC <= 2013.
  format_arg_store(const Args &... args) {
    value_type init[DATA_SIZE] =
      {internal::make_arg<IS_PACKED, Context>(args)...};
    std::memcpy(data_, init, sizeof(init));
  }
#else
  format_arg_store(const Args &... args)
    : data_{internal::make_arg<IS_PACKED, Context>(args)...} {}
#endif
};

#if !FMT_USE_CONSTEXPR11
template <typename Context, typename ...Args>
const unsigned long long format_arg_store<Context, Args...>::TYPES =
    get_types();
#endif

/**
  \rst
  Constructs an `~fmt::format_arg_store` object that contains references to
  arguments and can be implicitly converted to `~fmt::format_args`. `Context`
  can be omitted in which case it defaults to `~fmt::context`.
  \endrst
 */
template <typename Context = format_context, typename ...Args>
inline format_arg_store<Context, Args...>
  MakeFormatArgs(const Args &... args) { return {args...}; }

/** Formatting arguments. */
template <typename Context>
class basic_format_args {
 public:
  typedef unsigned size_type;
  typedef basic_format_arg<Context>  format_arg;

 private:
  // To reduce compiled code size per formatting function call, types of first
  // max_packed_args arguments are passed in the types_ field.
  unsigned long long types_;
  union {
    // If the number of arguments is less than max_packed_args, the argument
    // values are stored in values_, otherwise they are stored in args_.
    // This is done to reduce compiled code size as storing larger objects
    // may require more code (at least on x86-64) even if the same amount of
    // data is actually copied to stack. It saves ~10% on the bloat test.
    const internal::value<Context> *values_;
    const format_arg *args_;
  };

  bool is_packed() const { return (types_ & internal::is_unpacked_bit) == 0; }

  typename internal::type type(unsigned index) const {
    unsigned shift = index * 4;
    return static_cast<typename internal::type>(
      (types_ & (0xfull << shift)) >> shift);
  }

  friend class internal::arg_map<Context>;

  void set_data(const internal::value<Context> *values) { values_ = values; }
  void set_data(const format_arg *args) { args_ = args; }

  format_arg do_get(size_type index) const {
    format_arg arg;
    if (!is_packed()) {
      auto num_args = max_size();
      if (index < num_args)
        arg = args_[index];
      return arg;
    }
    if (index > internal::max_packed_args)
      return arg;
    arg.type_ = type(index);
    if (arg.type_ == internal::none_type)
      return arg;
    internal::value<Context> &val = arg.value_;
    val = values_[index];
    return arg;
  }

 public:
  basic_format_args() : types_(0) {}

  /**
   \rst
   Constructs a `basic_format_args` object from `~fmt::format_arg_store`.
   \endrst
   */
  template <typename... Args>
  basic_format_args(const format_arg_store<Context, Args...> &store)
  : types_(static_cast<unsigned long long>(store.TYPES)) {
    set_data(store.data_);
  }

  /**
   \rst
   Constructs a `basic_format_args` object from a dynamic set of arguments.
   \endrst
   */
  basic_format_args(const format_arg *args, size_type count)
    : types_(internal::is_unpacked_bit | count) {
    set_data(args);
  }

  /** Returns the argument at specified index. */
  format_arg get(size_type index) const {
    format_arg arg = do_get(index);
    if (arg.type_ == internal::named_arg_type)
      arg = arg.value_.as_named_arg().template deserialize<Context>();
    return arg;
  }

  size_type max_size() const {
    unsigned long long max_packed = internal::max_packed_args;
    return static_cast<size_type>(
      is_packed() ? max_packed : types_ & ~internal::is_unpacked_bit);
  }
};

/** An alias to ``basic_format_args<context>``. */
// It is a separate type rather than a typedef to make symbols readable.
struct format_args : basic_format_args<format_context> {
  template <typename ...Args>
  format_args(Args &&... arg)
  : basic_format_args<format_context>(std::forward<Args>(arg)...) {}
};
struct wformat_args : basic_format_args<wformat_context> {
  template <typename ...Args>
  wformat_args(Args &&... arg)
  : basic_format_args<wformat_context>(std::forward<Args>(arg)...) {}
};

#define FMT_ENABLE_IF_T(B, T) typename std::enable_if<B, T>::type

namespace internal {
template <typename Char>
struct named_arg_base {
  Baroque::StringView name;

  // Serialized value<context>.
  mutable char data[
    sizeof(basic_format_arg<typename buffer_context<Char>::type>)];

  named_arg_base(Baroque::StringView nm) : name(nm) {}

  template <typename Context>
  basic_format_arg<Context> deserialize() const {
    basic_format_arg<Context> arg;
    std::memcpy(&arg, data, sizeof(basic_format_arg<Context>));
    return arg;
  }
};

template <typename T, typename Char>
struct named_arg : named_arg_base<Char> {
  const T &value;

  named_arg(Baroque::StringView name, const T &val)
    : named_arg_base<Char>(name), value(val) {}
};

template <typename... Args, typename S>
inline typename std::enable_if<!is_compile_string<S>::value>::type
  check_format_string(const S &) {}
template <typename... Args, typename S>
typename std::enable_if<is_compile_string<S>::value>::type
  check_format_string(S);

template <typename S, typename... Args>
struct checked_args : format_arg_store<typename buffer_context<char>::type, Args...>
{
  typedef typename buffer_context<char>::type context;

  checked_args(const S &format_str, const Args &... args):
    format_arg_store<context, Args...>(args...) {
    internal::check_format_string<Args...>(format_str);
  }

  basic_format_args<context> operator*() const { return *this; }
};

Baroque::String vformat(
  Baroque::StringView format_str,
  basic_format_args<typename buffer_context<char>::type> args);

template<typename StringType>
StringType vformat(Baroque::StringView format_str, basic_format_args<typename buffer_context<char>::type> args);

typename buffer_context<char>::type::iterator VFormatTo(
	internal::basic_buffer<char> &buf, Baroque::StringView format_str,
  basic_format_args<typename buffer_context<char>::type> args);
}

/**
  \rst
  Returns a named argument to be used in a formatting function.

  **Example**::

    fmt::print("Elapsed time: {s:.2f} seconds", fmt::arg("s", 1.23));
  \endrst
 */
template <typename T>
inline internal::named_arg<T, char> Arg(Baroque::StringView name, const T &arg) {
  return {name, arg};
}

// Disable nested named arguments, e.g. ``arg("a", arg("b", 42))``.
template <typename S, typename T, typename Char>
void arg(S, internal::named_arg<T, Char>) = delete;

template <typename Container>
struct is_contiguous: std::false_type {};

template <typename Allocator>
struct is_contiguous<Baroque::StringImplementation<Allocator>>: std::true_type {};

template <typename Char>
struct is_contiguous<internal::basic_buffer<Char> >: std::true_type {};

/** Formats a string and writes the output to ``out``. */
template <typename Container, typename S>
typename std::enable_if<
    is_contiguous<Container>::value, std::back_insert_iterator<Container>>::type
  VFormatTo(
    std::back_insert_iterator<Container> out,
    const S &format_str,
    basic_format_args<typename buffer_context<char>::type> args) {
  internal::container_buffer<Container> buf(internal::get_container(out));
  internal::VFormatTo(buf, ToStringView(format_str), args);
  return out;
}

template <typename Container, typename S, typename... Args>
inline typename std::enable_if<
  is_contiguous<Container>::value && internal::is_string<S>::value,
  std::back_insert_iterator<Container>>::type
    FormatTo(std::back_insert_iterator<Container> out, const S &format_str,
              const Args &... args) {
  internal::checked_args<S, Args...> ca(format_str, args...);
  return VFormatTo(out, ToStringView(format_str), *ca);
}

template <typename S>
inline Baroque::String VFormat(const S &format_str, basic_format_args<typename buffer_context<char>::type> args)
{
	return internal::vformat(ToStringView(format_str), args);
}

template<typename StringType, typename S>
inline StringType VFormat(const S& format_str, basic_format_args<typename buffer_context<char>::type> args)
{
	return internal::vformat<StringType>(ToStringView(format_str), args);
}

/**
  \rst
  Formats arguments and returns the result as a string.

  **Example**::

    #include <fmt/core.h>
    std::string message = fmt::format("The answer is {}", 42);
  \endrst
*/
template <typename S, typename... Args>
inline Baroque::String Format(
    const S &format_str, const Args &... args) {
  return internal::vformat(
    ToStringView(format_str),
    *internal::checked_args<S, Args...>(format_str, args...));
}

template <typename StringType, typename S, typename... Args>
inline StringType Format(const S& format_str, const Args& ... args)
{
	return internal::vformat<StringType>(
		ToStringView(format_str),
		*internal::checked_args<S, Args...>(format_str, args...));
}

FMT_API void VPrint(std::FILE *f, Baroque::StringView format_str, format_args args);

/**
  \rst
  Prints formatted data to the file *f*. For wide format strings,
  *f* should be in wide-oriented mode set via ``fwide(f, 1)`` or
  ``_setmode(_fileno(f), _O_U8TEXT)`` on Windows.

  **Example**::

    fmt::print(stderr, "Don't {}!", "panic");
  \endrst
 */
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(internal::is_string<S>::value, void)
    Print(std::FILE *f, const S &format_str, const Args &... args) {
  VPrint(f, ToStringView(format_str),
         internal::checked_args<S, Args...>(format_str, args...));
}

FMT_API void VPrint(Baroque::StringView format_str, format_args args);

/**
  \rst
  Prints formatted data to ``stdout``.

  **Example**::

    fmt::print("Elapsed time: {0:.2f} seconds", 1.23);
  \endrst
 */
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(internal::is_string<S>::value, void)
    Print(const S &format_str, const Args &... args) {
  VPrint(ToStringView(format_str),
         internal::checked_args<S, Args...>(format_str, args...));
}
FMT_END_NAMESPACE
