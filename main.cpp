#include <iostream>
#include <tuple>
#include <cassert>

using namespace std;

enum Type_ {INT, FLOAT};

struct Type { Type_ t; };
struct Int : public Type {
  Int() { t = INT; }
};
struct Float : public Type {
  Float() { t = FLOAT; }
};

enum Expr_ {VALUE, ADD};

template <typename... Ls>
struct Expr {
  Expr(std::tuple<Ls...> extensions) {
    this->extensions = extensions;
  };

protected:
  Expr(Expr& other) = default;
  Expr(Expr&& other) = default;
  Expr& operator=(Expr& other) = default;
  Expr& operator=(Expr&& other) = default;

public:
  Expr_ t;
  std::tuple<Ls...> extensions;
};

template <typename... Ts>
struct Value : public Expr<Ts...> {
  Value(double val,
        std::tuple<Ts...> extensions) : Expr<Ts...>(extensions), val(val) {
    this->t = VALUE;
  };

  double val;
};

auto value(double val) -> Value<> {
  return Value<>(val, std::tuple<>());
}

template <typename... Ts>
struct Add;

template <template <typename...> class L,
          template <typename...> class R,
          typename... Ls,
          typename... Rs,
          typename... Ts>
struct Add<L<Ls..., Ts...>, R<Rs..., Ts...>, Ts...> : public Expr<Ts...> {
  Add(L<Ls...> lhs, R<Rs...> rhs,
      std::tuple<Ts...> extensions) : Expr<Ts...>(extensions),
                                      lhs(lhs), rhs(rhs) {
    this->t = ADD;
  };

  L<Ls...> lhs;
  R<Rs...> rhs;
};

template <class L, class R>
auto add(L lhs, R rhs) -> Add<L, R> {
  return Add<L, R>(lhs, rhs, std::tuple<>());
}

template <typename... Ts>
auto addType(Value<Ts...> val) -> Value<Ts..., Type> {
  auto extensions = std::tuple_cat(val.extensions, std::tuple<Type>(Float()));
  return Value<Ts..., Type>(val.val, extensions);
}

template <template <typename...> class L,
          template <typename...> class R,
          typename... Ls,
          typename... Rs,
          typename... Ts>
auto addType(Add<L<Ls...>, R<Rs...>, Ts...> add) -> Add<L<Ls..., Type>, R<Rs..., Type>, Ts..., Type> {
  auto extensions = std::tuple_cat(add.extensions, std::tuple<Type>(Int()));
  auto lhs = addType(add.lhs);
  auto rhs = addType(add.rhs);
  return Add<L<Ls..., Type>, R<Rs..., Type>, Ts..., Type>(lhs, rhs, extensions);
}

template <typename... Ts>
auto print(Value<Ts...> val) -> void {
  std::cout << val.val;
}

template <typename L, typename R, typename... Ts>
auto print(Add<L, R, Ts...> add) -> void {
  std::cout << "add(";
  print(add.lhs);
  std::cout << ", ";
  print(add.rhs);
  std::cout << ")";
}

auto main() -> int {
  cout << "Hello, World!" << endl;

  {
    auto v = value(1);
    print(v);
    std::cout << "\n";
  }

  auto expr = add(value(1), add(value(2), value(3)));

  print(expr); std::cout << "\n";

  auto withTypes = addType(expr);

//  print(withTypes); std::cout << "\n";

  cout << "Done, World!" << endl;
  return 0;
}

