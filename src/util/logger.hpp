#ifndef __UTIL_LOGGER_HPP
#define __UTIL_LOGGER_HPP

#include <ostream>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/tss.hpp>

namespace util { namespace logger
{

// adapted from:
//
// http://stackoverflow.com/questions/5528207/thread-safe-streams-and-stream-manipulators
//
// std::flush and std::endl are incompatible with the stream,
// the provided logger::flush and logger::endl must be used instead.
//
// redirects automatically to std::clog if unable to open / write to log file
// 

class Logger : boost::noncopyable
{
  std::string path;
  boost::mutex outMutex;
  std::ostream* out;
  boost::thread_specific_ptr<std::ostringstream> buffer;

  std::string Timestamp();
  Logger& Flush(bool newLine);
  
public:
  Logger();
  Logger(const std::string& path);
  ~Logger();

  void SetPath(const std::string& path);
  
  template <typename T> Logger& operator<<(T data) {
    if (!buffer.get()) buffer.reset(new std::ostringstream);                      
    std::ostringstream* oss = buffer.get();                                       
    (*oss) << data;                                                               
    return *this;                                                                 
  }

  Logger& operator<<(std::ostream& (*pf)(std::ostream&));
  Logger& operator<<(Logger& (*pf)(Logger&));
  
  const std::string& Path() const { return path; }
  
  friend Logger& flush(Logger& logger);
  friend Logger& endl(Logger& logger);
};

Logger& flush(Logger& logger);
Logger& endl(Logger& logger);

} /* logger namespace */
} /* util namespace */

#endif
