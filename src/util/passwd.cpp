#include <sstream>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include "util/passwd.hpp"
#include "util/verify.hpp"

namespace util { namespace passwd
{

std::string GenerateSalt(const unsigned int length)
{
  std::vector<unsigned char> salt;
  salt.reserve(length);
  verify(RAND_bytes(salt.data(), length));
  return std::string(salt.begin(), salt.end());
}

std::string HashPassword(const std::string& password, const std::string& salt)
{
  static const int iterations = 1000;
  
  std::vector<unsigned char> usalt(salt.begin(), salt.end());
  unsigned char key[32];
  
  verify(PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), 
                           usalt.data(), usalt.size(), iterations, 
                           EVP_sha256(), sizeof(key), key));
        
  return std::string(std::begin(key), std::end(key));
}

std::string HexEncode(const std::string& s)
{
  std::ostringstream os;
  os << std::hex << std::setfill('0') << std::uppercase;
  for (unsigned ch : s) 
  {
    os << std::setw(2) << ch;
  }
  return os.str();
}

std::string HexDecode(const std::string& s)
{
  std::ostringstream os;
  std::istringstream is1(s);
  std::string part;
  part.resize(2);
  while (is1.read(&part.front(), 2))
  {
    unsigned ch;
    std::istringstream is2(part);
    is2 >> std::hex >> ch;
    os << char(ch);
  }
  return os.str();
}

} /* crypto namespace */
} /* util namespace */
