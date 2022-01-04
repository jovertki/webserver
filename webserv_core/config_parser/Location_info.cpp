#include "Location_info.hpp"

Location_info::Location_info() : autoindex(NOT_ASSIGN), bodySize(NOT_ASSIGN), uploadPath(), index(), methods(),
                                returnNum() {}

std::ostream &operator<<(std::ostream &os, const Location_info &info) {
    os << "autoindex " << info.autoindex << ", bodySize " << info.bodySize << ", uploadPath " << info.uploadPath
       << ", index " << info.index << ", methods " << info.methods.size() << ", errorPage " << info.errorPage.size()
       << ", returnNum " << info.returnNum.size();
    return os;
}
