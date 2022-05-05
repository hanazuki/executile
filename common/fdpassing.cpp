// SPDX-License-Identifier: Zlib
#include "fdpassing.hpp"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>

#include <unistd.h>

#include "envvar.hpp"

namespace {
  std::string const LISTEN_FDS = "LISTEN_FDS", LISTEN_PID = "LISTEN_PID",
                    LISTEN_FDNAMES = "LISTEN_FDNAMES";
}

namespace executile::fdpassing {

  struct error: std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  void push_fd(int const fd, std::string_view const fdname) {

    if(fd < 0) {
      throw std::logic_error{"Invalid fd"};
    }

    if(!fdname_is_valid(fdname)) {
      throw error{"Invalid fdname"};
    }

    int listen_fds = 0;
    std::optional<std::string> listen_fdnames;

    auto const pid = std::to_string(getpid());
    if(auto const listen_pid = envvar::get(LISTEN_PID); !listen_pid || pid != *listen_pid) {
      // When LISTEN_PID is not set or set for another process, just reset the variables

      envvar::unset(LISTEN_FDS);
      envvar::unset(LISTEN_FDNAMES);
      envvar::set(LISTEN_PID, pid.c_str(), true);

    } else {
      // When LISTEN_PID is set for this process, read and verify the variables

      if(auto const s = envvar::get(LISTEN_FDS)) {
        if(auto const [ptr, ec] = std::from_chars(s->begin(), s->end(), listen_fds);
           ptr != s->end() || ec != std::errc()) {
          throw error{"Invalid value for LISTEN_FDS"};
        }
      } else {
        throw error{"LISTEN_PID is set for this process but LISTEN_FDS is not set"};
      }

      if(auto const s = envvar::get(LISTEN_FDNAMES)) {
        listen_fdnames = s;

        auto const c = std::count(listen_fdnames->begin(), listen_fdnames->end(), FDNAME_DELIMITOR);
        if(c != listen_fds - 1) {
          throw error{"Number of LISTEN_FDNAMES does not match with LISTEN_FDS"};
        }
      }
    }

    if(fd != SD_LISTEN_FDS_START + listen_fds) {
      throw std::logic_error{"Unexpected fd"};
    }

    ++listen_fds;

    if(listen_fdnames) {
      listen_fdnames->append(1, FDNAME_DELIMITOR).append(fdname);
    } else if(!fdname.empty()) {
      listen_fdnames = std::string(listen_fds - 1, FDNAME_DELIMITOR).append(fdname);
    }

    envvar::set(LISTEN_FDS, std::to_string(listen_fds), true);
    if(listen_fdnames) {
      envvar::set(LISTEN_FDNAMES, *listen_fdnames, true);
    }
  }
}
