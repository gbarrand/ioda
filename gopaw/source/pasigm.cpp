// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pasigm_(void*);

  int sigma_C(const char*,void*); 
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/yacc/cexpr_eval.h>

#include "session"

#include <inlib/touplow>
#include <inlib/srep>
#include <inlib/sout>

//static 
//bool SIGMA(gopaw::session&,const std::string&);
//So that Shark find the source code of SIGMA.
extern "C" {
  bool SIGMA(gopaw::session&,const std::string&);
}

namespace gopaw {

inline bool set_array(vector_t& aArray,const std::vector<unsigned int>& aIS,const std::vector<double>& aVec) {
  if(aArray.orders()!=aIS) return false;
  std::vector<double>& astdv = aArray.vector();
  int sz = aVec.size();
  for(int index=0;index<sz;index++) astdv[index] = aVec[index];
  return true;
} 

}

int sigma_C(const char* a_script,void* a_tag)  { 
  // Called by kuip when doing $SIGMA(<exp>)
  // It is used in PAW.cpp to initialize KUIP/SIGMA through a call to kusigm.
  // It is the equivalent of CERNLIBG/pawlib/sigma/src/sigmae.F
  gopaw::session& _sess = *((gopaw::session*)a_tag);
  //printf("debug : sigma_C : try to proceed \"%s\"...\n",a_script);
  SIGMA(_sess,a_script); 
  //return 1; //Failure
  return 0; //Success
}

void pasigm_(void* a_tag) { 

  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pasigm : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/FORTRAN/SIGMA") {

    std::string EXPR;
    int argc = ku_npar();
    for(int index=0;index<argc;index++) EXPR += ku_gets();

    SIGMA(_sess,EXPR); 

  } else {
    out << "pasigm : " << cmd_path << " : dummy." << std::endl;
  }
}

bool SIGMA(gopaw::session& a_session,const std::string& a_script) {
  if(a_script.empty()) return true;

  std::ostream& out = a_session.out();

  std::string::size_type eq = a_script.find('=');
  if(eq==std::string::npos) {
    out << "SIGMA : no assignement in " << inlib::sout(a_script) << std::endl;
    return false;
  }
  
  std::string assign = a_script.substr(0,eq);
  inlib::strip(assign);
  inlib::touppercase(assign);

  std::string script = a_script.substr(eq+1,a_script.size()-(eq+1));  
  inlib::tolowercase(script);
  inlib::replace(script,'#',','); //For ARRAY(x,y#z)

  gopaw::ceval_t& processor = a_session.processor();

  processor.set_script(script);

  std::vector<std::string> names;
  if(!processor.find_variables(names)) {
    out << "SIGMA : can't find variables in " << inlib::sout(a_script) << std::endl;
    return false;
  }
  if(!processor.compile(names)) {
    out << "SIGMA : compilation of " << inlib::sout(a_script) << " failed." << std::endl;
    return false;
  }

  // Get variables :
  inlib::params<std::string,gopaw::value_t>& variables = a_session.variables();

  std::vector<gopaw::value_t> vals;
  std::vector<unsigned int> orders;
  bool foundOrders = false;
  inlib_vforcit(std::string,names,it) {
    std::string name = *it;
    inlib::touppercase(name);
    if(name=="PI") {
      vals.push_back(gopaw::value_t(inlib::pi()));
      continue;
    }
    gopaw::vector_t* vector = 0;
    if(!a_session.find_vector(name,vector)) {
      if(!variables.is_key(name)) {  // Search in sSIGMA variables :
        out << "SIGMA : can't find variable " << inlib::sout(name)
            << " in script " << inlib::sout(a_script) << "."
            << std::endl;
        return false;
      }
      vals.push_back(variables.value(name));
    } else {
      if(!foundOrders) {
        orders = vector->orders();
        foundOrders = true;
      } else {
        if(vector->orders()!=orders) {
          out << "SIGMA : orders mismatch." << std::endl;
          return false;
        }
      }
      vals.push_back(gopaw::value_t(*vector)); //to optimize (here we copy vector data).
    }
  }

 {if(!a_session.remove_vector(assign)) {
    //out << "SIGMA : remove " << inlib::sout(assign) << " failed." << std::endl;
  }
  if(variables.is_key(assign)) variables.remove(assign);}

  gopaw::value_t var;
  std::string serror;
  if(!processor.execute(vals,var,serror)) {
    out << "SIGMA : execution of " << inlib::sout(a_script) << " failed." << std::endl;
    out << serror << std::endl;
    return false;
  }
  if(serror==gopaw::value_t::error_div_zero()) {
    out << "SIGMA : execution of " << inlib::sout(a_script) << " has divide by zero errors, but continue anyway." << std::endl;
  }

  if(var.type()==gopaw::value_t::DOUBLE) {

    variables.add(0,assign,"",var,gopaw::value_t(),gopaw::value_t()); 

  } else if(var.type()==gopaw::value_t::UNSIGNED_INT) {

    variables.add(0,assign,"",gopaw::value_t((double)var.get_unsigned_int()),gopaw::value_t(),gopaw::value_t());    

  } else if(var.type()==gopaw::value_t::INT) {

    variables.add(0,assign,"",gopaw::value_t((double)var.get_int()),gopaw::value_t(),gopaw::value_t());    

  } else if(var.type()==gopaw::value_t::ARRAY_DOUBLE) {

    std::vector<unsigned int> is;
    const std::vector<double>& stdv = var.get_array_double(is);

    gopaw::vector_t* vassign = new gopaw::vector_t(is);
    if(!vassign) {
      out << "SIGMA : can't create vector " << inlib::sout(assign) << " in script " << inlib::sout(a_script) << "." << std::endl;
      return false;
    } 
    vassign->vector() = stdv;
    a_session.add_vector(assign,vassign);

  } else {
    out << "SIGMA : result not a vector or a double in script " << inlib::sout(a_script) << "." << std::endl;
    return false;
  }

  return true;
}

