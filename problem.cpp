#include <iostream>

struct Expr {};

struct Num : public Expr {
  Num(int n) : n(n) {};
  int n;
};

Num num(int n) {
  return Num{n};
}

int eval(Num n) {
  return n.n;
}

void foo1() {
  auto e = num(1);
  std::cout << eval(e) << "\n";
}

#include <type_traits>

template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B,T>::type;

template <typename L, typename R,
          typename = enable_if_t<std::is_base_of<Expr, L>::value>,
          typename = enable_if_t<std::is_base_of<Expr, R>::value>>
struct Plus : public Expr {
  Plus(const L& l, const R& r) : l(l), r(r) {};
  L l;
  R r;
};

template <typename L, typename R>
Plus<L, R> plus(L l, R r) {
  return Plus<L, R>{l, r};
}

template <typename L, typename R>
int eval(Plus<L, R> p) {
  return eval(p.l) + eval(p.r);
}

void foo2() {
  auto e = plus(num(1), num(2));
  std::cout << eval(e) << "\n";
}

void print(Num n) {
  std::cout << n.n;
}

template <typename L, typename R>
void print(Plus<L, R> p) {
  std::cout << "(";
  print(p.l);
  std::cout << ") + (";
  print(p.r);
  std::cout << ")";
}

void foo3() {
  auto e = plus(num(1), plus(num(2), num(3)));
  print(e); std::cout << "\n";
}

int main() {
  foo1();
  foo2();
  foo3();

  std::cout << "Done!\n";
  return 0;
}

