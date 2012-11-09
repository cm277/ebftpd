#ifndef __DB_BSON_MESSAGE_HPP
#define __DB_BSON_MESSAGE_HPP

#include <mongo/client/dbclient.h>

namespace db { namespace mail
{
class Message;
}

namespace bson
{

struct Message
{
  static mongo::BSONObj Serialize(const db::mail::Message& message);
  static db::mail::Message Unserialize(const mongo::BSONObj& bo);
};

} /* bson namespace */
} /* db namespace */

#endif
