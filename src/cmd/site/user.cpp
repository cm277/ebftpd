#include <sstream>
#include "cmd/site/user.hpp"
#include "acl/types.hpp"
#include "acl/user.hpp"
#include "acl/usercache.hpp"
#include "db/user/userprofile.hpp"
#include "acl/userprofile.hpp"
#include "acl/groupcache.hpp"
#include "db/user/userprofile.hpp"
#include "db/group/group.hpp"
#include "util/error.hpp"
#include "acl/allowsitecmd.hpp"

namespace cmd { namespace site
{

cmd::Result USERCommand::Execute()
{
  if (args.size() == 2 && args[1] != client.User().Name() && 
      !acl::AllowSiteCmd(client.User(), "user"))
  {
    return cmd::Result::Permission;
  }

  acl::User user(client.User());
  if (args.size() == 2)
  {
    try
    {
      user = acl::UserCache::User(args[1]);
    }
    catch (const util::RuntimeError& e)
    {
      control.Reply(ftp::ActionNotOkay, "Error: " + e.Message());
      return cmd::Result::Okay;
    }
  }

  acl::UserProfile profile;
  try
  {
    profile = db::userprofile::Get(user.UID());
  }
  catch (const util::RuntimeError&e )
  {
    control.Reply(ftp::ActionNotOkay, "Error: " + e.Message());
    return cmd::Result::Okay;
  }
  
  std::string creator = "<ebftpd>";
  try
  {
    acl::User creatorUser = acl::UserCache::User(profile.Creator());
    creator = creatorUser.Name();
  }
  catch (const util::RuntimeError& e)
  {
    if (profile.Creator() != 0) creator = "<deleted>";
  }

  std::ostringstream os;
  os.imbue(std::locale(""));

  os << "+=======================================================================+";
  os << "\n| Username: " << user.Name() << "\tLogged in " 
     << profile.LoggedIn() << " times.";
  os << "\n| Created: " << profile.Created();
  os << "\n| Last login: " << profile.LastLogin(); 
  
  const std::string& expires = profile.Expires();
  if (expires != "not-a-date-time")
    os << "\n| Expires: " << expires;
  os << "\n| Created by: " << creator;
  os << "\n| Flags: " << user.Flags();
  os << "\n| Ratio: " << profile.Ratio();
  os << "\n| Credits: ";

  long long credits = user.Credits()/1000;
  os << credits << " MiB";
 
  std::string group = (user.PrimaryGID() == -1) ? "NoGroup" : acl::GroupCache::Group(user.PrimaryGID()).Name();
  os << "\n| Primary Group: " << group;

  auto secondary = user.SecondaryGIDs();
  if (secondary.size() > 0)
  {
    os << "\n| Secondary Groups: ";
    acl::Group group;
    for (auto& gid: user.SecondaryGIDs())
    {
      try
      {
        group = acl::GroupCache::Group(gid);
        os << "\n|\t" << group.Name();
      }
      catch (const util::RuntimeError& e)
      {
        os << "\n| Error: " << e.Message();
      }
    }
  }

  os << "\n| Tagline: " << profile.Tagline();
  os << "\n| Comment: " << profile.Comment();
  if (profile.WeeklyAllotment() == 0)
    os << "\n| Weekly Allotment: <unlimited>";
  else
    os << "\n| Weekly Allotment: " << profile.WeeklyAllotment() / 1024 << "MB";
  os << "\n+=======================================================================+";

  control.MultiReply(ftp::CommandOkay, os.str());
  return cmd::Result::Okay;
}


// end
}
}

