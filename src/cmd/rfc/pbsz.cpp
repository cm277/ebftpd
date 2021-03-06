#include "cmd/rfc/pbsz.hpp"

namespace cmd { namespace rfc
{

cmd::Result PBSZCommand::Execute()
{
  if (args[1] != "0")
    control.Reply(ftp::ParameterNotImplemented, "Only protection buffer size 0 supported.");
  else
    control.Reply(ftp::CommandOkay, "Protection buffer size set to 0.");
  return cmd::Result::Okay;
}

} /* rfc namespace */
} /* cmd namespace */
