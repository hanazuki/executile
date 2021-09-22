// SPDX-License-Identifier: Zlib
#pragma once

#include <stdexcept>
#include <system_error>
#include <netdb.h>

namespace executile {

  struct address_info {
    address_info(address_info &&);
    ~address_info();

    address_info(address_info const &) = delete;
    address_info &operator=(address_info const &) = delete;

    static address_info get(char const *node, char const *service, addrinfo const &hints);

    struct iterator {
      addrinfo const &operator*() const;
      iterator &operator++();

    private:
      iterator(addrinfo *cursor);
      addrinfo *cursor;

      friend struct address_info;
      friend bool operator==(iterator const &, iterator const &);
      friend bool operator!=(iterator const &, iterator const &);
    };

    iterator begin() const;
    iterator end() const;

  private:
    address_info(addrinfo *head);

    addrinfo *head_;
  };

  bool operator==(address_info::iterator const &, address_info::iterator const &);
  bool operator!=(address_info::iterator const &, address_info::iterator const &);

  struct gaierror: std::runtime_error {
    gaierror(int errcode);
  };
}
