#pragma once

#include <system_error>

#include <sys/socket.h>

namespace executile {
  struct socket {
    socket(int domain, int type, int protocol);
    socket(socket &&);
    ~socket();

    socket(socket const &) = delete;
    socket &operator=(socket const &) = delete;

    int fd() const;

    template <typename V> void setsockopt(int level, int optname, V const &opt) {
      if(::setsockopt(fd_, level, optname, &opt, sizeof(opt)) != 0) {
        throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "setsockopt"};
      }
    }

    void bind(sockaddr const *addr, socklen_t addrlen);
    void listen(int backlog);

  private:
    int fd_;
  };

}
