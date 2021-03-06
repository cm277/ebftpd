#include <boost/lexical_cast.hpp>
#include "cmd/site/logs.hpp"
#include "logs/logs.hpp"
#include "util/reverselogreader.hpp"

namespace cmd { namespace site
{

bool LOGSCommand::ParseArgs()
{
  int n = 1;
  boost::to_lower(args[n]);
  if (args[n] == "-max")
  {
    if (args.size() < 4) return false;
    try
    {
      number = boost::lexical_cast<int>(args[n + 1]);
    }
    catch (const boost::bad_lexical_cast&)
    { return false; }
    if (number <= 0) return false;
    n += 2;
    boost::to_lower(args[n]);
  }
  
  log = args[n];
  strings.assign(args.begin() + n + 1, args.end());
  return true;
}

bool LOGSCommand::CheckStrings(const std::string& line)
{
  for (auto& s : strings)
    if (line.find_first_of(s) == std::string::npos)
      return false;
  return true;
}

void LOGSCommand::Show(const std::string& path)
{
  try
  {
    util::ReverseLogReader in(path);
    std::string line;
    int count = 0;
    while (in.Getline(line) && count < number)
    {
      if (CheckStrings(line))
      {
        control.PartReply(ftp::CommandOkay, line);
        ++count;
      }
    }
  
    control.Reply(ftp::CommandOkay, "LOGS command finished");
  }
  catch (const util::SystemError& e)
  {
    control.Reply(ftp::ActionNotOkay, "Unable to opne log: " + e.Message());
    return;
  }
}

cmd::Result LOGSCommand::Execute()
{ 
  if (!ParseArgs()) return cmd::Result::SyntaxError;

  if (log == "error") Show(logs::error.Path());
  else if (log == "security") Show(logs::security.Path());
  else if (log == "sysop") Show(logs::sysop.Path());
  else if (log == "events") Show(logs::events.Path());
  else if (log == "db") Show(logs::db.Path());
  else return cmd::Result::SyntaxError;
  
  return cmd::Result::Okay;
}

} /* site namespace */
} /* cmd namespace */
