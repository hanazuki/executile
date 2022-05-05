// SPDX-License-Identifier: Zlib

#include <optional>
#include <string>
#include <string_view>

namespace executile::envvar {
  void set(std::string const &name, std::string const &value, bool overwrite);
  void unset(std::string const &name);
  std::optional<std::string_view> get(std::string const &name);
}
