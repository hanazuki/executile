// SPDX-License-Identifier: Zlib
#include "envvar.hpp"

#include <system_error>

#include <errno.h>
#include <string.h>
#include <unistd.h>

namespace executile::envvar {

  void set(std::string const &name, std::string const &value, bool const overwrite) {
    if(setenv(name.c_str(), value.c_str(), overwrite) != 0) {
      throw std::system_error(errno, std::generic_category(), "setenv");
    }
  }

  void unset(std::string const &name) {
    if(unsetenv(name.c_str()) != 0) {
      throw std::system_error(errno, std::generic_category(), "unsetenv");
    }
  }

  std::optional<std::string_view> get(std::string const &name) {
    if(auto const p = getenv(name.c_str())) {
      return p;
    }
    return std::nullopt;
  }

}
