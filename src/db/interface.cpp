#include <ctime>
#include <mongo/client/dbclient.h>
#include <boost/thread/future.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "db/interface.hpp"
#include "db/task.hpp"
#include "db/pool.hpp"
#include "db/types.hpp"
#include "db/bson/user.hpp"
#include "db/bson/group.hpp"
#include "acl/types.hpp"
#include "acl/usercache.hpp"
#include "acl/groupcache.hpp"
#include "acl/ipmaskcache.hpp"
#include "acl/types.hpp"
#include "logs/logs.hpp"

namespace db
{

void Initalize()
{
  db::Pool::StartThread();
  acl::GroupCache::Initalize();
  acl::UserCache::Initalize();
  acl::IpMaskCache::Initalize();

  std::vector<TaskPtr> tasks;
  
  tasks.emplace_back(new db::EnsureIndex("users",
    BSON("uid" << 1 << "name" << 1)));
  tasks.emplace_back(new db::EnsureIndex("groups",
    BSON("gid" << 1 << "name" << 1)));
  tasks.emplace_back(new db::EnsureIndex("transfers", BSON("uid" << 1 << 
    "direction" << 1 << "day" << 1 << "week" << 1 << "month" << 1 << 
    "year" << 1)));
  tasks.emplace_back(new db::EnsureIndex("ipmasks",
    BSON("uid" << 1 << "mask" << 1)));

  for (auto& task: tasks)
    Pool::Queue(task);
    
  acl::UserCache::Create("biohazard", "password", "1");
  acl::UserCache::Create("io", "password", "1");
}

// end
}
