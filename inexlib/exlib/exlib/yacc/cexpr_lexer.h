// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#ifndef exlib_yacc_cexpr_lexer_h
#define exlib_yacc_cexpr_lexer_h

#include <vector>
#include <string>
#include <ostream>
#include <cstring>
#include <cstddef>

namespace exlib {
namespace yacc {

class cexpr_lexer {
public:
  cexpr_lexer(std::ostream& a_out)
  :m_current(0)
  ,m_end(0)
  ,m_out(a_out)
  {}
  virtual ~cexpr_lexer(){}
public:
  cexpr_lexer(const cexpr_lexer& a_from)
  :m_current(0)
  ,m_end(0)
  ,m_out(a_from.m_out)
  {}

  cexpr_lexer& operator=(const cexpr_lexer& /*a_from*/){
    m_string.clear();
    m_current = 0;
    m_end = 0;
    m_variables.clear();
    return *this;
  }

public:
  void set(const std::string& aString) {
    m_string = aString;
    m_current = (char*)m_string.c_str();
    m_end = m_current + ::strlen(m_current);
  }

  std::vector<std::string>& variables(){return m_variables;}

  const std::string& variable(unsigned int a_index) const {
    return m_variables[a_index];
  }

  std::ostream& out() const {return m_out;}
public:
  size_t input(char* a_buffer,size_t a_size) {
    if(a_size<=0) return 0;
    ptrdiff_t rest = m_end - m_current;
    size_t n = ptrdiff_t(a_size)>rest ? size_t(rest) : a_size;
    if(n>0) {
      ::memcpy(a_buffer,m_current,n);
      m_current += n;
    }
    return n;
  }

protected:
  std::string m_string;
  char* m_current;
  char* m_end;
  std::vector<std::string> m_variables;
  std::ostream& m_out;
};

}}

#endif
