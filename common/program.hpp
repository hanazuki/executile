// SPDX-License-Identifier: Zlib
#pragma once

#include <span>
#include <vector>

namespace executile {
  struct program {
    program(std::span<char *> args);
    void execute() const;

  private:
    std::vector<char *> args_;
  };
}
