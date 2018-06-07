
#include <string>
#include <vector>

namespace Lib {

class System {
public:
  static bool kind(std::string&);
  static bool isKindOf(const std::string&);
  static bool getenv(const std::string&,std::string&);
};

class smanip {
public:
  static std::string removeVersions(const std::string&);
  static std::string dirname(const std::string&,unsigned int = 1);
  static std::string basename(const std::string&);

  static bool getenv(std::string&);
  static std::vector<std::string> words(const std::string&,const std::string&,bool=false);
  static bool pathNameSuffix(const std::string&,
                     std::string&,std::string&,std::string&);
  static bool replace(std::string&,const std::string&,const std::string&);
  static bool isAbsolutePath(const std::string&);
  static bool version(const std::string&,std::vector<unsigned int>&);
};

class dirmanip {
public:
  static bool binaryPath(const std::string&,std::string&);
  static bool directory(std::string&);
};

class fmanip {
public:
  static bool findResource(const std::string&,const std::string&,
                           std::string&);

  static std::string name(const std::string&);
  static bool read(const std::string&,std::vector<std::string>&);
};

}

