#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>                                   
#include <boost/bind.hpp>                                     
#include "cfg/config.hpp"
#include "cfg/exception.hpp"
#include "cfg/setting.hpp"
#include "util/string.hpp"
#include "logger/logger.hpp"

namespace cfg
{

int Config::latestVersion = 0;

Config::Config(const std::string& config) : version(++latestVersion), config(config)
{
  std::string line;
  std::ifstream io(config.c_str(), std::ifstream::in);
  int i = 0;

  if (!io.is_open()) throw ConfigFileError();

  while (io.good())
  {
    std::getline(io, line);
    ++i;
    if (line.size() == 0) continue;
    if (line.size() > 0 && line.at(0) == '#') continue;
    try 
    {
      Parse(line);
    } 
    catch (NoSetting &e) // handle properly
    {
      ::logger::ftpd << e.what() << " (" << config << ":" << i << ")" << logger::endl;
    }
    catch (...)
    {
      logger::ftpd << "super error on line " << i << logger::endl;
      logger::ftpd << line << logger::endl;
      throw;
    }
  }

  SanityCheck();
}

void Config::Parse(const std::string& line) {
  std::vector<std::string> toks;
  boost::split(toks, line, boost::is_any_of("\t "), boost::token_compress_on);
  for (std::vector<std::string>::iterator it = toks.begin(); it != toks.end();
    ++it)
    boost::replace_all((*it), "[:space:]", " ");
  
  if (toks.size() == 0) return;
  std::string opt = toks.at(0);
  if (opt.size() == 0) return;
  // remove setting from args
  toks.erase(toks.begin());
  std::vector<std::string>::iterator it;
  for (it = toks.begin(); it != toks.end();)
    if ((*it).size() == 0)
      it = toks.erase(it);
    else
      ++it;
    

  // parse string
  boost::algorithm::to_lower(opt);

  // plan to rehaul this area in the future to sway from glftpd's inconsitencies
  // check if we have a perm to parse
  if (opt.at(0) == '-' || opt.find("custom-") != std::string::npos)
  {
    std::vector<std::string> temp;
    boost::split(temp, opt, boost::is_any_of("-"));
    if (temp.size() > 1) temp.erase(temp.begin());
    // loop and update
    return;
  }

  if (opt == "sitepath")
  {
    sitepath = fs::Path(toks.at(0));
  }
  else if (opt == "listen_addr")
  {
    listenAddr = toks[0];
  }
  else if (opt == "port")
  {
    port = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "tls_certificate")
  {
    tlsCertificate = fs::Path(toks.at(0));
  }
  else if (opt == "rootpath")
  {
    rootpath = fs::Path(toks.at(0));
  }
  else if (opt == "reload_config")
  {
    reloadConfig = fs::Path(toks.at(0));
  }
  else if (opt == "datapath")
  {
    datapath = fs::Path(toks.at(0));
  }
  else if (opt == "pwd_path")
  {
    pwdPath = fs::Path(toks.at(0));
  }
  else if (opt == "grp_path")
  {
    grpPath = fs::Path(toks.at(0));
  }
  else if (opt == "botscript_path")
  {
    botscriptPath = fs::Path(toks.at(0));
  }
  else if (opt == "banner")
  {
    banner = fs::Path(toks.at(0));
  }
  else if (opt == "ascii_downloads")
  {
    asciiDownloads = setting::AsciiDownloads(toks);
  }
  else if (opt == "free_space")
  {
    freeSpace = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "mmap_amount")
  {
    mmapAmount = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "dl_send_file")
  {
    dlSendFile = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "ul_buffered_force")
  {
    ulBufferedForce = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "total_users")
  {
    totalUsers = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "multiplier_max")
  {
    multiplierMax = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "oneliners")
  {
    oneliners = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "empty_nuke")
  {
    emptyNuke = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "max_sitecmd_lines")
  {
    maxSitecmdLines = boost::lexical_cast<int>(toks.at(0));
  }
  else if (opt == "shutdown")
  {
    shutdown = acl::ACL::FromString(boost::algorithm::join(toks, " ")); 
  }
  else if (opt == "hideuser")
  {
    hideuser = acl::ACL::FromString(boost::algorithm::join(toks, " "));
  }
  else if (opt == "use_dir_size")
  {
    useDirSize.push_back(setting::UseDirSize(toks)); 
  }
  else if (opt == "timezone")
  {
    timezone = setting::Timezone(toks);
  }
  else if (opt == "color_mode")
  {
    colorMode = util::string::BoolLexicalCast(toks.at(0));
  }
  else if (opt == "dl_incomplete")
  {
    dlIncomplete = util::string::BoolLexicalCast(toks.at(0));
  }
  else if (opt == "file_dl_count")
  {
    fileDlCount = util::string::BoolLexicalCast(toks.at(0));
  }
  else if (opt == "sitename_long")
  {
    sitenameLong = toks[0];
  }
  else if (opt == "sitename_short")
  {
    sitenameShort = toks[0];
  }
  else if (opt == "login_prompt")
  {
    loginPrompt = toks[0];
  }
  else if (opt == "email")
  {
    email = toks[0];
  }
  else if (opt == "master")
  {
    master = toks; 
  }
  else if (opt == "bouncer_ip")
  {
    bouncerIp = toks;
  }
  else if (opt == "calc_crc")
  {
    calcCrc = toks;
  }
  else if (opt == "xdupe")
  {
    xdupe = toks;
  }
  else if (opt == "valid_ip")
  {
    validIp = toks;
  }
  else if (opt == "active_addr")
  {
    activeAddr = toks; 
  }
  else if (opt == "ignore_type")
  { 
    for (std::vector<std::string>::iterator it = toks.begin(); it != toks.end();
      ++it)
      ignoreType.push_back((*it));
  }
  else if (opt == "banned_users")
  {
    bannedUsers = toks;
  }
  else if (opt == "idle_commands")
  {
    idleCommands = toks;
  }
  else if (opt == "noretrieve")
  {
    for (std::vector<std::string>::iterator it = toks.begin(); it != toks.end();
      ++it)
      ignoreType.push_back((*it));
  }
  else if (opt == "tagline")
  {
    tagline.push_back(toks[0]);
  }
  else if (opt == "speed_limit")
  {
    speedLimit.push_back(setting::SpeedLimit(toks));
  }
  else if (opt == "sim_xfers")
  {
    simXfers.push_back(setting::SimXfers(toks));
  }
  else if (opt == "secure_ip")
  {
    secureIp.push_back(setting::SecureIp(toks));
  }
  else if (opt == "secure_pass")
  {
    securePass.push_back(setting::SecurePass(toks));
  }
  else if (opt == "pasv_addr")
  {
    pasvAddr.push_back(setting::PasvAddr(toks));
  }
  else if (opt == "active_ports")
  {
    activePorts = setting::Ports(toks);
  }
  else if (opt == "pasv_ports")
  {
    pasvPorts = setting::Ports(toks);
  }
  else if (opt == "allow_fxp")
  {
    allowFxp.push_back(setting::AllowFxp(toks));
  }
  else if (opt == "welcome_msg")
  {
    welcomeMsg.push_back(fs::Path(toks.at(0))); 
  }
  else if (opt == "goodbye_msg")
  {
    goodbyeMsg.push_back(fs::Path(toks.at(0))); 
  }
  else if (opt == "newsfile")
  {
    newsfile.push_back(fs::Path(toks.at(0))); 
  }
  else if (opt == "cdpath")
  {
    cdpath.push_back(fs::Path(toks.at(0)));
  }
  else if (opt == "nodupecheck")
  {
    nodupecheck.push_back(fs::Path(toks.at(0)));
  }
  else if (opt == "alias")
  {
    alias.push_back(setting::Alias(toks)); 
  }
  else if (opt == "delete")
  {
    delete_.push_back(setting::Right(toks));
  }
  else if (opt == "deleteown")
  {
    deleteown.push_back(setting::Right(toks));
  }
  else if (opt == "overwrite")
  {
    overwrite.push_back(setting::Right(toks));
  }
  else if (opt == "resume")
  {
    resume.push_back(setting::Right(toks));
  }
  else if (opt == "rename")
  {
    rename.push_back(setting::Right(toks));
  }
  else if (opt == "renameown")
  {
    renameown.push_back(setting::Right(toks));
  }
  else if (opt == "filemove")
  {
    filemove.push_back(setting::Right(toks));
  }
  else if (opt == "makedir")
  {
    makedir.push_back(setting::Right(toks));
  }
  else if (opt == "upload")
  {
    upload.push_back(setting::Right(toks));
  }
  else if (opt == "download")
  {
    download.push_back(setting::Right(toks));
  }
  else if (opt == "nuke")
  {
    nuke.push_back(setting::Right(toks));
  }
  else if (opt == "dirlog")
  {
    dirlog.push_back(setting::Right(toks));
  }
  else if (opt == "hideinwho")
  {
    hideinwho.push_back(setting::Right(toks));
  }
  else if (opt == "freefile")
  {
    freefile.push_back(setting::Right(toks));
  }
  else if (opt == "nostats")
  {
    nostats.push_back(setting::Right(toks));
  }
  else if (opt == "show_diz")
  {
    showDiz.push_back(setting::Right(toks));
  }
  else if (opt == "stat_section")
  {
    statSection.push_back(setting::StatSection(toks));
  }
  else if (opt == "path-filter")
  {
    pathFilter.push_back(setting::PathFilter(toks));
  }
  else if (opt == "max_users")
  {
    maxUsers.push_back(setting::MaxUsers(toks));
  }
  else if (opt == "max_ustats")
  {
    maxUstats = setting::ACLInt(toks);
  }
  else if (opt == "max_gstats")
  {
    maxGstats = setting::ACLInt(toks);
  }
  else if (opt == "show_totals")
  {
    showTotals.push_back(setting::ShowTotals(toks)); 
  }
  else if (opt == "dupe_check")
  {
    dupeCheck = setting::DupeCheck(toks);
  }
  else if (opt == "script")
  {
    script.push_back(setting::Script(toks));
  }
  else if (opt == "lslong")
  {
    lslong = setting::Lslong(toks);
  }
  else if (opt == "hidden_files")
  {
    hiddenFiles.push_back(setting::HiddenFiles(toks));
  }
  else if (opt == "creditcheck")
  {
    creditcheck.push_back(setting::Creditcheck(toks));
  }
  else if (opt == "creditloss")
  {
    creditloss.push_back(setting::Creditloss(toks));
  }
  else if (opt == "nukedir_style")
  {
    nukedirStyle = setting::NukedirStyle(toks);
  }
  else if (opt == "privgroup")
  {
    privgroup.push_back(setting::Privgroup(toks));
  }
  else if (opt == "msg_path")
  {
    msgpath.push_back(setting::Msgpath(toks));
  }
  else if (opt == "privpath")
  {
    privpath.push_back(setting::Privpath(toks)); 
  }
  else if (opt == "site_cmd")
  {
    siteCmd.push_back(setting::SiteCmd(toks));
  }
  else if (opt == "cscript")
  {
    cscript.push_back(setting::Cscript(toks));
  }
  else if (opt == "requests")
  {
    requests = setting::Requests(toks);
  }

  // update cache for sanity check
  settingsCache[opt]++; 
}

bool Config::CheckSetting(const std::string& name)
{
  std::tr1::unordered_map<std::string, int>::const_iterator it;
  it = settingsCache.find(name);
  return (it != settingsCache.end());
}

void Config::SanityCheck()
{
  // required
  if (!CheckSetting("tls_certificate")) throw RequiredSetting("tls_certificate");
  else if (!CheckSetting("sitepath")) throw RequiredSetting("sitepath");
  else if (!CheckSetting("port")) throw RequiredSetting("port");
  else if (!CheckSetting("listen_addr")) throw RequiredSetting("listen_addr");

}

// end namespace
}

#ifdef CFG_CONFIG_TEST
int main()
{
  try
  {
    cfg::Config config("ftpd.conf");
    logger::ftpd << "Config loaded." << logger::endl;
    logger::ftpd << "Download: " << config.Download().size() << logger::endl;
  }
  catch(const cfg::ConfigError& e)
  {
    logger::ftpd << e.what() << logger::endl;
    return 1;
  }
  return 0;
}
#endif
  
