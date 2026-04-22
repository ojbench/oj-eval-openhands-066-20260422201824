#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <initializer_list>
#include <typeinfo>
#include <utility>
#include <cstddef>
#include <string>

namespace sjtu {

class any_ptr {
 private:
  struct control_block {
    void *ptr{nullptr};
    const std::type_info *ti{nullptr};
    void (*deleter)(void *){nullptr};
    std::size_t refcnt{1};
  };

  control_block *ctrl_{nullptr};

  template <class T>
  static void destroy(void *p) {
    delete static_cast<T *>(p);
  }

  void retain() {
    if (ctrl_) ++ctrl_->refcnt;
  }

  void release() {
    if (ctrl_) {
      if (--ctrl_->refcnt == 0) {
        if (ctrl_->deleter && ctrl_->ptr) ctrl_->deleter(ctrl_->ptr);
        delete ctrl_;
      }
      ctrl_ = nullptr;
    }
  }

 public:
  any_ptr() {}

  any_ptr(const any_ptr &other) : ctrl_(other.ctrl_) { retain(); }

  template <class T>
  explicit any_ptr(T *ptr) {
    if (ptr) {
      ctrl_ = new control_block{static_cast<void *>(ptr), &typeid(T), &destroy<T>, 1};
    }
  }

  ~any_ptr() { release(); }

  any_ptr &operator=(const any_ptr &other) {
    if (this == &other) return *this;
    release();
    ctrl_ = other.ctrl_;
    retain();
    return *this;
  }

  template <class T>
  any_ptr &operator=(T *ptr) {
    if (ctrl_ && ctrl_->ptr == ptr && ctrl_->ti == &typeid(T)) return *this;
    release();
    if (ptr) ctrl_ = new control_block{static_cast<void *>(ptr), &typeid(T), &destroy<T>, 1};
    return *this;
  }

  template <class T>
  T &unwrap() {
    if (!ctrl_ || !ctrl_->ptr || *(ctrl_->ti) != typeid(T)) throw std::bad_cast();
    return *static_cast<T *>(ctrl_->ptr);
  }

  template <class T>
  const T &unwrap() const {
    if (!ctrl_ || !ctrl_->ptr || *(ctrl_->ti) != typeid(T)) throw std::bad_cast();
    return *static_cast<const T *>(ctrl_->ptr);
  }
};

template <class T>
any_ptr make_any_ptr(const T &t) {
  return any_ptr(new T(t));
}

template <class T, class... Args>
any_ptr make_any_ptr(Args &&...args) {
  return any_ptr(new T{std::forward<Args>(args)...});
}

template <class T>
any_ptr make_any_ptr(std::initializer_list<typename T::value_type> il) {
  return any_ptr(new T(il));
}

// Convenience overload for string literals
template <std::size_t N>
any_ptr make_any_ptr(const char (&s)[N]) {
  return any_ptr(new std::string(s));
}

}  // namespace sjtu

#endif
