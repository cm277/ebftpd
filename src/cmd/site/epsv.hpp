#ifndef __CMD_SITE_EPSV_HPP
#define __CMD_SITE_EPSV_HPP

#include "cmd/command.hpp"

namespace cmd { namespace site
{

class EPSVCommand : public Command
{
public:
  EPSVCommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    Command(client, client.Control(), client.Data(), argStr, args) { }

  cmd::Result Execute();
};

} /* site namespace */
} /* cmd namespace */

#endif
