#ifndef __ACL_PATH_HPP
#define __ACL_PATH_HPP

#include <string>
#include "util/error.hpp"

namespace acl
{ 

class User;

namespace path
{

enum Type
{
  Upload,
  Resume,
  Makedir,
  Download,
  Dirlog,
  Rename,
  Filemove,
  Nuke,
  Delete,
  View,
  Hideinwho,
  Freefile,
  Nostats,
  Hideowner
};

template <Type type>
util::Error FileAllowed(const User& user, const std::string& path);

template <Type type>
util::Error DirAllowed(const User& user, std::string path);

} /* path namespace */
} /* acl namespace */

#endif
