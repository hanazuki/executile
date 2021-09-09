#pragma once

#include <string_view>

#ifndef SD_LISTEN_FDS_START
#define SD_LISTEN_FDS_START 3
#endif

namespace executile::fdpassing {
  constexpr char FDNAME_DELIMITOR = ':';

  constexpr bool fdname_is_valid(std::string_view fdname) {
    if(fdname.size() >= 255) {
      return false;
    }

    for(auto const c: fdname) {
      if(c < 32 || c >= 127 || c == FDNAME_DELIMITOR) {
        return false;
      }
    }

    return true;
  }

  void push_fd(int fd, std::string_view fdname = "");
}
