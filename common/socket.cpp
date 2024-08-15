// SPDX-License-Identifier: Zlib
#include "socket.hpp"

#include <system_error>

#include <sys/socket.h>
#include <unistd.h>

namespace executile {

  socket::socket(int domain, int type, int protocol): fd_(::socket(domain, type, protocol)) {
    if(fd_ == -1) {
      throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "socket"};
    }
  }

  socket::socket(socket &&other): fd_(-1) {
    std::swap(fd_, other.fd_);
  }

  socket::~socket() {
    if(fd_ != -1) {
      close(fd_);
      fd_ = -1;
    }
  }

  void socket::bind(sockaddr const *addr, socklen_t addrlen) {
    if(::bind(fd_, addr, addrlen) != 0) {
      throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "bind"};
    }
  }

  void socket::listen(int backlog) {
    if(::listen(fd_, backlog) != 0) {
      throw std::system_error{std::make_error_code(static_cast<std::errc>(errno)), "listen"};
    }
  }

  int socket::fd() const {
    return fd_;
  }

}
