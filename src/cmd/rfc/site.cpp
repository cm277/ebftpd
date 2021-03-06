#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "cmd/rfc/site.hpp"
#include "cmd/site/factory.hpp"
#include "cfg/get.hpp"
#include "acl/allowsitecmd.hpp"
#include "cmd/splitargs.hpp"

namespace cmd { namespace rfc
{

cmd::Result SITECommand::Execute()
{
  cmd::SplitArgs(argStr, args);
  boost::to_upper(args[0]);
  argStr = argStr.substr(args[0].length());
  boost::trim(argStr);

  cmd::site::CommandDefOptRef def(cmd::site::Factory::Lookup(args[0]));
  if (!def)
  {
    control.Reply(ftp::CommandUnrecognised, "Command not understood");
  }
  else if (!acl::AllowSiteCmd(client.User(), def->ACLKeyword()))
  {
    control.Reply(ftp::ActionNotOkay,  "SITE " + args[0] + ": Permission denied");
  }
  else if (!def->CheckArgs(args))
  {
    control.Reply(ftp::SyntaxError, def->Syntax());
  }
  else
  {
    cmd::CommandPtr command(def->Create(client, argStr, args));
    if (!command)
    {
      control.Reply(ftp::NotImplemented, "Command not implemented");
    }
    else
    {
      cmd::Result result = command->Execute();
      if (result == cmd::Result::SyntaxError)
        control.Reply(ftp::SyntaxError, def->Syntax());
      else
      if (result == cmd::Result::Permission)
        control.Reply(ftp::ActionNotOkay, "SITE " + args[0] + ": Permission denied");
    }
  }
  return cmd::Result::Okay;
}

} /* rfc namespace */
} /* cmd namespace */
