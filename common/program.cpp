// SPDX-License-Identifier: Zlib
#include "program.hpp"

#include <system_error>

#include <unistd.h>

namespace executile {
  program::program(std::span<char *> args) {
    args_ = {args.begin(), args.end()};
    args_.push_back(nullptr);
  }

  void program::execute() const {
    execvp(args_.front(), args_.data());
    throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "execvp"};
  }
}
