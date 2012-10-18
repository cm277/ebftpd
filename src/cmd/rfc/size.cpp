#include <boost/lexical_cast.hpp>
#include "cmd/rfc/size.hpp"
#include "acl/path.hpp"
#include "cfg/get.hpp"

namespace cmd { namespace rfc
{

cmd::Result SIZECommand::Execute()
{
  namespace PP = acl::path;

  fs::Path absolute = (client.WorkDir() / argStr).Expand();

  util::Error e(PP::FileAllowed<PP::View>(client.User(), absolute));
  if (!e)
  {
    control.Reply(ftp::ActionNotOkay, "SIZE failed 2: " + e.Message());
    return cmd::Result::Okay;
  }
  
  fs::Status status;
  try
  {
    status.Reset(cfg::Get().Sitepath() + absolute);
  }
  catch (const util::SystemError& e)
  {
    control.Reply(ftp::ActionNotOkay, "SIZE failed 1: " + e.Message());
    return cmd::Result::Okay;
  }
  
  control.Reply(ftp::FileStatus, boost::lexical_cast<std::string>(status.Size())); 
  return cmd::Result::Okay;
}

} /* rfc namespace */
} /* cmd namespace */
