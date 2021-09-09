#include "address_info.hpp"

#include <stdexcept>

#include <netdb.h>

namespace executile {

  address_info::address_info(::addrinfo *head): head_(head) {
  }

  address_info::address_info(address_info &&other) {
    std::swap(head_, other.head_);
  }

  address_info::~address_info() {
    freeaddrinfo(head_);
    head_ = nullptr;
  }

  address_info address_info::get(char const *node, char const *service, addrinfo const &hints) {
    addrinfo *p;

    if(auto const r = getaddrinfo(node, service, &hints, &p); r != 0) {
      if(r == EAI_SYSTEM) {
        throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "getaddrinfo"};
      }
      throw gaierror{r};
    }

    return {p};
  }

  address_info::iterator::iterator(addrinfo *cursor): cursor(cursor) {
  }

  addrinfo const &address_info::iterator::operator*() const {
    return *cursor;
  }

  address_info::iterator &address_info::iterator::operator++() {
    ++cursor;
    return *this;
  }

  address_info::iterator address_info::begin() const {
    return {head_};
  }

  address_info::iterator address_info::end() const {
    return {nullptr};
  }

  bool operator==(address_info::iterator const &a, address_info::iterator const &b) {
    return a.cursor == b.cursor;
  }

  bool operator!=(address_info::iterator const &a, address_info::iterator const &b) {
    return !(a == b);
  }

  gaierror::gaierror(int errcode)
    : std::runtime_error{std::string("getaddrinfo: ") + gai_strerror(errcode)} {
  }

}
