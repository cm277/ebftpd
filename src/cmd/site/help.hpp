#ifndef __CMD_SITE_HELP_HPP
#define __CMD_SITE_HELP_HPP

#include <string>
#include "cmd/command.hpp"

namespace cmd { namespace site
{

class HELPCommand : public Command
{
  cmd::Result Syntax();
  cmd::Result List();

public:
  HELPCommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    Command(client, client.Control(), client.Data(), argStr, args) { }

  cmd::Result Execute();
};

} /* site namespace */
} /* cmd namespace */

#endif
