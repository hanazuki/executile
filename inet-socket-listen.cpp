#include <cerrno>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <bits/getopt_core.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/address_info.hpp"
#include "common/fdpassing.hpp"
#include "common/program.hpp"
#include "common/socket.hpp"

constexpr struct option long_options[] = {
  {"stream", no_argument, 0, 's'},
  {"datagram", no_argument, 0, 'd'},
  {"ipv4", no_argument, 0, '4'},
  {"ipv6", no_argument, 0, '6'},
  {"backlog", required_argument, 0, 'b'},
  {"name", required_argument, 0, 0},
  {"numeric-host", no_argument, 0, 1},
  {"numeric-service", no_argument, 0, 2},
  {"ipv6only", no_argument, 0, 3},
  {0, 0, 0, 0},
};

[[noreturn]] static void help(char const *progname) {
  std::cerr
    << "Usage: " << progname
    << " (--stream|--datagram) [-4|--ipv4|-6|--ipv6|--ipv6only] [--name=<name>] [--numeric-host] "
       "[--numeric-service] [--backlog=<backlog>] <host> <service> <prog> [<prog-args>...]"
    << std::endl;

  exit(EXIT_FAILURE);
}

int opt_socktype = 0;
int opt_family = 0;
std::string opt_name;
bool opt_numeric_host;
bool opt_numeric_service;
bool opt_ipv6only;
int opt_backlog;

static executile::socket listen_socket(char const *host, char const *service);

int main(int argc, char **argv) {
  if(argc == 0) {
    return EXIT_FAILURE;
  }

  int opt;
  while((opt = getopt_long(argc, argv, "+46", long_options, nullptr)) != -1) {
    switch(opt) {
    case 's':
      if(opt_socktype && opt_socktype != SOCK_STREAM) {
        std::cerr << "--stream and --datagram are exclusive" << std::endl;
        help(argv[0]);
      }
      opt_socktype = SOCK_STREAM;
      break;

    case 'd':
      if(opt_socktype && opt_socktype != SOCK_DGRAM) {
        std::cerr << "--stream and --datagram are exclusive" << std::endl;
        help(argv[0]);
      }
      opt_socktype = SOCK_DGRAM;
      break;

    case '4':
      if(opt_family && opt_family != AF_INET) {
        std::cerr << "--ipv4 and --ipv6 are exclusive" << std::endl;
        help(argv[0]);
      }
      opt_family = AF_INET;
      break;

    case '6':
      if(opt_family && opt_family != AF_INET6) {
        std::cerr << "--ipv4 and --ipv6 are exclusive" << std::endl;
        help(argv[0]);
      }
      opt_family = AF_INET6;
      break;

    case 'b': {
      std::string_view const s = optarg;
      if(auto const [ptr, ec] = std::from_chars(s.begin(), s.end(), opt_backlog);
         ptr != s.end() || ec != std::errc()) {
        std::cerr << "Invalid value for --backlog" << std::endl;
      }
      break;
    }

    case 0:
      opt_name = optarg;
      if(!executile::fdpassing::fdname_is_valid(opt_name)) {
        std::cerr << "Invalid fdname: " << opt_name << std::endl;
        help(argv[0]);
      }
      break;

    case 1:
      opt_numeric_host = true;
      break;

    case 2:
      opt_numeric_service = true;
      break;

    case 3:
      if(opt_family && opt_family != AF_INET6) {
        std::cerr << "--ipv4 and --ipv6only are exclusive" << std::endl;
        help(argv[0]);
      }
      opt_family = AF_INET6;
      opt_ipv6only = true;
      break;

    case '?':
      help(argv[0]);
    }
  }

  if(!opt_socktype) {
    std::cerr << "Either --stream or --datagram is required" << std::endl;
    help(argv[0]);
  }

  if(opt_socktype == SOCK_STREAM) {
    if(opt_backlog == 0) {
      opt_backlog = SOMAXCONN;
    }
  } else {
    if(opt_backlog != 0) {
      std::cerr << "--backlog is for --stream" << std::endl;
      help(argv[0]);
    }
  }

  if(argc - optind < 3) {
    std::cerr << "Insufficient arguments" << std::endl;
    help(argv[0]);
  }

  try {
    auto socket = listen_socket(argv[optind], argv[optind + 1]);

    executile::fdpassing::push_fd(socket.fd(), opt_name);

    executile::program({argv + optind + 2, argv + argc}).execute();

  } catch(std::exception const &ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
}

executile::socket listen_socket(char const *host, char const *service) {
  addrinfo hints = {
    .ai_family = opt_family,
    .ai_socktype = opt_socktype,
  };
  if(opt_numeric_host) {
    hints.ai_flags |= AI_NUMERICHOST;
  }
  if(opt_numeric_service) {
    hints.ai_flags |= AI_NUMERICSERV;
  }

  for(auto const ai: executile::address_info::get(host, service, hints)) {
    executile::socket s{
      ai.ai_family,
      ai.ai_socktype,
      ai.ai_protocol,
    };

    if(opt_ipv6only) {
      s.setsockopt<int>(IPPROTO_IPV6, IPV6_V6ONLY, 1);
    }

    s.bind(ai.ai_addr, ai.ai_addrlen);

    if(opt_socktype == SOCK_STREAM) {
      s.listen(opt_backlog);
    }

    return s;
  }

  throw std::runtime_error{"No address available"};
}
