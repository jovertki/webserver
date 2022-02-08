#include "Location_info.hpp"

Location_info::Location_info() : autoIndex(NOT_ASSIGN), bodySize(NOT_ASSIGN), root(), uploadPath(), index(), methods(),
                                 redirection(std::make_pair(0, "")) {}

std::ostream &operator<<(std::ostream &os, const Location_info &info) {
    os << "autoindex " << info.autoIndex << ", bodySize " << info.bodySize << ", root " << info.root << ", uploadPath " << info.uploadPath
       << ", index " << info.index << ", methods " << info.methods.size() << ", errorPage " << info.errorPage.size()
       << ", returnNum " << info.redirection.first;
    return os;
}
