// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  int vector_create_C(void*);
  int vector_delete_C(void*);
  int vector_read_C(void*);
  int vector_write_C(void*);
  int vector_input_C(void*);
  int vector_list_C(void*);
  int vector_operation_C(void*);
  int vector_print_C(void*);
  int vector_find_C(const char*,void*);
  int vector_value_C(const char*,double*,void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>

#include "session"

#include <exlib/KUIP/tools>

#include <inlib/sout>
#include <inlib/smatch>
#include <inlib/parse>
#include <inlib/system>
#include <inlib/mnmx>
#include <inlib/arrout>

#include <exlib/yacc/fort_out>

int vector_create_C(void* a_tag){
  // From KUIP/kvect.c/kxvcreate
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();
  
  if(_sess.verbose_level()) {
    out << "vector_create_C : execute." << std::endl;
  }

  std::string VNAME = ku_gets();
  std::string TYPE = ku_gets();
  std::string VALUES = ku_gets();

  //printf("debug : \"%s\" \"%s\" \"%s\"\n",
  // VNAME.c_str(),TYPE.c_str(),VALUES.c_str());

  if( (TYPE!="I") && (TYPE!="i") && (TYPE!="R") && (TYPE!="r") ) {
    //if implementing, change also the ret value of vector_find_C
    out << "vector_create_C : TYPE " << inlib::sout(TYPE) << " not supported." << std::endl;
    return gopaw::status_failure();
  }

  //bool real = ((TYPE=="R") || (TYPE=="r") ? true : false);

  std::string NAME;
  std::vector< std::pair<int,int> > dims;
  if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return gopaw::status_failure();
  inlib::touppercase(NAME);

  // Create :
  if(!dims.size()) {
    dims.push_back( std::pair<int,int>(0,0));
  }
  unsigned int dimension = dims.size();
  std::vector<unsigned int> orders(dimension);
  std::vector< std::pair<unsigned int,unsigned int> > cut;
  for(unsigned int index = 0;index<dimension;index++) {
    //FIXME : not clear if range are accepted in the CREATE command.
    //if(dims[index].first!=dims[index].second) {
    //  out << "vector_create_C :" 
    //            << " Bad format in " << inlib::sout(VNAME) 
    //      << " but continue anyway." << std::endl;
    //}
    orders[index] = dims[index].second+1;
    if(dims[index].second==inlib::not_found()) {
      out << "vector_create_C : have a inlib::not_found() max." << std::endl;
      return gopaw::status_failure();
    }
    cut.push_back(std::pair<unsigned int,unsigned int>(0,dims[index].second));
  }

  if(_sess.remove_vector(NAME)) {
    //out << " *** VECTOR/CREATE " << NAME
    //    << ": existing vector " << NAME << " replaced"
    //    << std::endl;
  }

  gopaw::vector_t* vector = new gopaw::vector_t(orders);
  if(!vector) {
    out << "vector_create_C : can't create vector " << inlib::sout(NAME) << "." << std::endl;
    return gopaw::status_failure();
  }
  _sess.add_vector(NAME,vector);

  // Values :
  std::vector<double> values;
  if(!inlib::to_doubles(VALUES,values,",")) {
    out << "vector_create_C : can't convert " << inlib::sout(VALUES) << " to doubles." << std::endl;
  } else {
    if(values.size()) {
      if(!vector->fill(values,&cut)) {
        out << "vector_create_C : vector.fill returns false." << std::endl;
      }
    }
  }

  if(_sess.verbose_level()) {
    //array_dump(out,*vector,std::string("vector ")+NAME+" :");
    out << "vector " << NAME << " :" << std::endl;
    if(!inlib::array_print(out,*vector,dims,NAME,/*DENSE*/2)) {
      out << "vector_C : array_print failed for " << inlib::sout(VNAME) << std::endl;
    }
  }

  return gopaw::status_success();
}

int vector_delete_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) {
    out << "vector_delete_C : execute." << std::endl;
  }

  std::string VLIST = ku_gets();

  inlib::touppercase(VLIST);
  std::vector<std::string> V_LIST;
  inlib::words(VLIST,",",false,V_LIST);

  inlib_vforcit(std::string,V_LIST,it) {

    std::string::size_type star = (*it).find('*');
    if(star==std::string::npos){
      if(!_sess.remove_vector(*it)) {
        //out << "vector_delete_C : remove " << inlib::sout(*it) << " failed." << std::endl;
      }
    } else { //wildcard.
      std::vector<gopaw::named_vector>& vecs = _sess.named_vectors();
      std::vector<gopaw::named_vector>::iterator itv;
      for(itv=vecs.begin();itv!=vecs.end();) {
        std::string& name = (*itv).first;
        //printf("debug : %s %s\n",name.c_str(),type.c_str());
        if(inlib::match(name,*it)) {
          if(_sess.verbose_level()) {
            out << "vector_delete_C : "  << inlib::sout(name) << " matches " << inlib::sout(*it) << std::endl;
          }
          delete (*itv).second;
          itv = vecs.erase(itv);
        } else {
          ++itv;
        }
      }
    }

  }

  return gopaw::status_success();
}

int vector_read_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) {
    out << "vector_read_C : execute." << std::endl;
  }

  std::string VLIST = ku_gets(); //FIXME : handle list of vectors.
  std::string FNAME = ku_gets();
  //std::string FORMAT = ku_gets();
  //std::string CHOPT = ku_gets();
  //std::string MATCH = ku_gets();

  if(_sess.verbose_level()) {
    out << "vector_read_C :"
        << " VLIST= " << inlib::sout(VLIST) 
        << " FNAME= " << inlib::sout(FNAME) 
        << std::endl;
  }

  inlib::touppercase(VLIST);
  std::vector<std::string> V_LIST;
  inlib::words(VLIST,",",false,V_LIST);

  unsigned int coln = V_LIST.size();
  if(!coln) {
    out << "vector_read_C : VLIST empty." << std::endl;
    return gopaw::status_failure();
  }

  std::string sfile;
  inlib::file_name(FNAME,sfile); //to expand environment variables.

  std::vector<double> values;
  if(!inlib::read_doubles(sfile,values)) {
    out << "vector_read_C : can't read file " << inlib::sout(sfile) << ". FNAME is " << inlib::sout(FNAME) << "." << std::endl;
    return gopaw::status_failure();
  }

  if(_sess.verbose_level()) {
    out << "vector_read_C : file " << inlib::sout(sfile) << ". FNAME is " << inlib::sout(FNAME) << "." << std::endl;
    for(unsigned int index=0;index<values.size();index++) {
      out << values[index] << std::endl;
    }
  }

  unsigned int rows = values.size()/coln;
  if((rows*coln)!=values.size()) {
    out << "vector_read_C : too much data in file." << " Read only first " << (rows*coln) << " values." << std::endl;
  }

  for(unsigned int col=0;col<coln;col++) {
    std::string NAME = V_LIST[col];

    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(NAME,vector)) { //create it
      //out << " *** VECTOR/READ: Unknown vector " 
      //    << NAME << " is created with length        " 
      //    << rows << std::endl;
      std::vector<unsigned int> orders(1);
      orders[0] = rows;
      vector = new gopaw::vector_t(orders);
      if(!vector) {
        out << "vector_read_C : can't create vector " << inlib::sout(NAME) << "." << std::endl;
        return gopaw::status_failure();
      }
      _sess.add_vector(NAME,vector);
    }

    // brute force :
    std::vector<double> v_items(rows);
    for(unsigned int row=0;row<rows;row++) {
      v_items[row] = values[col+row*coln];
    }

    if(v_items.size()) {
      if(!vector->fill(v_items,0)) {
        out << "vector_read_C : vector.fill returns false." << std::endl;
      }
    }
  
    if(_sess.verbose_level()) {
      std::vector< std::pair<int,int> > dims;
      //array_dump(out,*vector,std::string("vector ")+NAME+" :");
      out << "vector " << NAME << " :" << std::endl;
      if(!inlib::array_print(out,*vector,dims,NAME,/*DENSE*/2)) {
        out << "vector_C : array_print failed for " << inlib::sout(NAME) << std::endl;
      }
    }
  }

  return gopaw::status_success();
}

#include <fstream>

int vector_write_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) { 
    out << "vector_wite_C : execute." << std::endl;
  }

  std::string VLIST = ku_gets();
  std::string FNAME = ku_gets();
  std::string FORMAT = ku_gets();
  std::string CHOPT = ku_gets();

  if(_sess.verbose_level()) {
    out << "vector_wite_C :" 
        << " VLIST= " << inlib::sout(VLIST) 
        << " FNAME = " << inlib::sout(FNAME) 
        << " FORMAT = " << inlib::sout(FORMAT) 
        << " CHOPT = " << inlib::sout(CHOPT) 
        << std::endl;
  }

  inlib::touppercase(CHOPT);
  std::vector<std::string> OPTS;
  exlib::KUIP::get_opts("CHOPT",OPTS);
  if(_sess.verbose_level()) {
    out << "vector_wite_C : CHOPT " << inlib::sout(CHOPT) << std::endl;
  }

  inlib::touppercase(VLIST);
  std::vector<std::string> V_LIST;
  inlib::words(VLIST,",",false,V_LIST);

  size_t coln = V_LIST.size();
  if(!coln) {
    out << "vector_write_C : VLIST empty." << std::endl;
    return gopaw::status_failure();
  }

  std::vector<gopaw::vector_t*> as(coln);

  size_t rows = 0;
 {for(size_t col=0;col<coln;col++) {
    std::string NAME = V_LIST[col];
    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(NAME,vector)) {
      out << "vector_write_C : can't find vector " << inlib::sout(NAME) << "." << std::endl;
      return gopaw::status_failure();
    }
    as[col] = vector;  
    const std::vector<double>& v = vector->vector();  
    if(col) {
      rows = inlib::mn<size_t>(rows,v.size());
    } else {
      rows = v.size();
    }
  }}

 {for(size_t col=0;col<coln;col++) {
     gopaw::vector_t* vector = as[col];
    const std::vector<double>& v = vector->vector();  
    if(v.size()!=rows) {
      out << "vector_write_C :" 
          << " mismatch size for vector " << inlib::sout(V_LIST[col]) 
          << ". It has " << v.size() << " entries, whilst "
          << rows << " are expected."
          << " Only " << rows << " are going to be written."
          << std::endl;
    }
  }}

  // brute force :
  std::vector<double> vout(rows*coln);
 {for(size_t row=0;row<rows;row++) {
    for(size_t col=0;col<coln;col++) {
      gopaw::vector_t* vector = as[col];
      const std::vector<double>& v = vector->vector();  
      vout[col+row*coln] = v[row];
    }
  }}

  std::string sfile;
  inlib::file_name(FNAME,sfile); //to expand environment variables.

  if(_sess.verbose_level()) {
    out << "vector_wite_C :" 
        << " FNAME = " << inlib::sout(FNAME) 
        << " path = " << inlib::sout(sfile) 
        << std::endl;
  }

  std::ofstream file_writer(sfile.c_str());
  if(!file_writer.is_open()) {
    out << "vector_write_C : can't open " << inlib::sout(sfile) << "." << std::endl;
    return gopaw::status_failure();
  }

  exlib::fort_out f77out(file_writer,out,"("+FORMAT+")");
  f77out << vout << exlib::fort_out::endl;
  file_writer.close();
  
  return gopaw::status_success();
}

int vector_input_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) {
    out << "vector_input_C : execute." << std::endl;
  }

  std::string VNAME = ku_gets();
  std::string VALUES = ku_gets();

  // Values :
  std::vector<double> values;
  if(!inlib::to_doubles(VALUES,values,",")) {
    out << "vector_input_C : can't convert " << inlib::sout(VALUES) << " to doubles." << std::endl;
    return gopaw::status_failure();
  }
  if(_sess.verbose_level()) {
    out << "vector_input_C : VALUES " << std::endl;
    for(unsigned int index=0;index<values.size();index++) {
      out << values[index] << std::endl;
    }
  }

  std::string NAME;
  std::vector< std::pair<int,int> > dims;
  if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return gopaw::status_failure();
  inlib::touppercase(NAME);
  gopaw::vector_t* vector = 0;
  if(!_sess.find_vector(NAME,vector)) {
    out << "vector_input_C : can't find vector " << inlib::sout(NAME) << std::endl;
    return gopaw::status_failure();
  } 
  std::vector< std::pair<unsigned int,unsigned int> > cut;
  if(!array_get_cut(*vector,dims,cut)) {
    out << "vector_input_C : mismatch dimension in " << inlib::sout(NAME) << std::endl;
    return gopaw::status_failure();
  }

  if(values.size()) {
    if(!vector->fill(values,&cut)) {
      out << "vector_input_C : vector.fill returns false." << std::endl;
    }
  }

  if(_sess.verbose_level()) {
    //array_dump(out,*vector,std::string("vector ")+NAME+" :");
    out << "vector " << NAME << " :" << std::endl;
    if(!inlib::array_print(out,*vector,dims,NAME,/*DENSE*/2)) {
      out << "vector_C : array_print failed for " << inlib::sout(VNAME) << std::endl;
    }
  }

  return gopaw::status_success();
}

int vector_operation_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "vector_operation_C : execute " << inlib::sout(cmd_name) << std::endl;
  }

  //cmd_path==/VECTOR/OPERATIONS/VBIAS
  //cmd_path==/VECTOR/OPERATIONS/VSCALE
  //cmd_path==/VECTOR/OPERATIONS/VADD
  //cmd_path==/VECTOR/OPERATIONS/VSUBTRACT
  //cmd_path==/VECTOR/OPERATIONS/VMULTIPLY
  //cmd_path==/VECTOR/OPERATIONS/VDIVIDE

  if( (cmd_name=="VBIAS") || (cmd_name=="VSCALE") )  {

    std::string VNAME1 = ku_gets();
    double BIAS = ku_getr();
    double SCALE = BIAS;
    std::string VNAME2 = ku_gets();

    std::string NAME1;
    std::vector< std::pair<int,int> > dims1;
    if(!gopaw::session::parse_name(VNAME1,NAME1,dims1,out)) return gopaw::status_failure();
    inlib::touppercase(NAME1);
    gopaw::vector_t* vec1 = 0;
    if(!_sess.find_vector(NAME1,vec1)) {
      out << "vector_operation_C : can't find vector " << inlib::sout(NAME1) << "." << std::endl;
      return gopaw::status_failure();
    }
    std::vector< std::pair<unsigned int,unsigned int> > cut1;
    if(!array_get_cut(*vec1,dims1,cut1)) {
      out << "vector_operation_C : mismatch dimension in " << inlib::sout(VNAME1) << std::endl;
      return gopaw::status_failure();
    }

    std::string NAME2;
    std::vector< std::pair<int,int> > dims2;
    if(!gopaw::session::parse_name(VNAME2,NAME2,dims2,out)) return gopaw::status_failure();
    inlib::touppercase(NAME2);

    if(cmd_name=="VBIAS") {
      if(NAME2==NAME1) { //operation on itself
        vec1->add(BIAS,&cut1);
      } else {
        if(!_sess.remove_vector(NAME2)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME2) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C : can't add vector " << inlib::sout(NAME1) << "." << std::endl;
          return gopaw::status_failure();
        }
        vector->add(BIAS);
        _sess.add_vector(NAME2,vector);
      }

    } else if(cmd_name=="VSCALE") {  

      if(NAME2==NAME1) { //operation on itself
        vec1->multiply(SCALE);
      } else {
        if(!_sess.remove_vector(NAME2)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME2) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C : can't scale vector " << inlib::sout(NAME1) << "." << std::endl;
          return gopaw::status_failure();
        }
        vector->multiply(BIAS);
        _sess.add_vector(NAME2,vector);
      }

    }
    

  } else if( (cmd_name=="VADD") || 
             (cmd_name=="VSUBTRACT") ||
             (cmd_name=="VMULTIPLY") ||
             (cmd_name=="VDIVIDE") )  {

    std::string VNAME1 = ku_gets();
    std::string VNAME2 = ku_gets();
    std::string VNAME3 = ku_gets();

    std::string NAME1;
    std::vector< std::pair<int,int> > dims1;
    if(!gopaw::session::parse_name(VNAME1,NAME1,dims1,out)) return gopaw::status_failure();
    inlib::touppercase(NAME1);
    gopaw::vector_t* vec1 = 0;
    if(!_sess.find_vector(NAME1,vec1)) {
      out << "vector_operation_C : can't find vector " << inlib::sout(NAME1) << "." << std::endl;
      return gopaw::status_failure();
    }
    std::vector< std::pair<unsigned int,unsigned int> > cut1;
    if(!array_get_cut(*vec1,dims1,cut1)) {
      out << "vector_operation_C : mismatch dimension in " << inlib::sout(VNAME1) << std::endl;
      return gopaw::status_failure();
    }

    std::string NAME2;
    std::vector< std::pair<int,int> > dims2;
    if(!gopaw::session::parse_name(VNAME2,NAME2,dims2,out)) return gopaw::status_failure();
    inlib::touppercase(NAME2);
    gopaw::vector_t* vec2 = 0;
    if(!_sess.find_vector(NAME2,vec2)) {
      out << "vector_operation_C : can't find vector " << inlib::sout(NAME2) << "." << std::endl;
      return gopaw::status_failure();
    }
    std::vector< std::pair<unsigned int,unsigned int> > cut2;
    if(!array_get_cut(*vec2,dims2,cut2)) {
      out << "vector_operation_C : mismatch dimension in " << inlib::sout(VNAME2) << std::endl;
      return gopaw::status_failure();
    }

    std::string NAME3;
    std::vector< std::pair<int,int> > dims3;
    if(!gopaw::session::parse_name(VNAME3,NAME3,dims3,out)) return gopaw::status_failure();
    inlib::touppercase(NAME3);

    if(cmd_name=="VADD") {

      if(NAME3==NAME1) { //operation on itself
        // V1 + V2 = V1
        vec1->add(*vec2);
      } else if(NAME3==NAME2) { //operation on itself
        // V1 + V2 = V2
        vec2->add(*vec1);
      } else {
        if(!_sess.remove_vector(NAME3)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME3) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C :" 
              << " can't add vector " 
              << inlib::sout(NAME1) << " with "
              << inlib::sout(NAME2) << "."
              << std::endl;
          return gopaw::status_failure();
        }
        vector->add(*vec2);
        _sess.add_vector(NAME3,vector);
      }

    } else if(cmd_name=="VSUBTRACT") {

      if(NAME3==NAME1) { //operation on itself
        // V1 - V2 = V1
        vec1->subtract(*vec2);
      } else if(NAME3==NAME2) { //operation on itself
        // V1 - V2 = V2
        vec2->multiply(-1);
        vec2->add(*vec1);
      } else {
        if(!_sess.remove_vector(NAME3)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME3) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C :" 
              << " can't subtract vector " 
              << inlib::sout(NAME1) << " with "
              << inlib::sout(NAME2) << "."
              << std::endl;
          return gopaw::status_failure();
        }
        vector->subtract(*vec2);
        _sess.add_vector(NAME3,vector);
      }

    } else if(cmd_name=="VMULTIPLY") {

      if(NAME3==NAME1) { //operation on itself
        // V1 * V2 = V1
        vec1->multiply(*vec2);
      } else if(NAME3==NAME2) { //operation on itself
        // V1 * V2 = V2
        vec2->multiply(*vec1);
      } else {
        if(!_sess.remove_vector(NAME3)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME3) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C :" 
              << " can't multiply vector " 
              << inlib::sout(NAME1) << " with "
              << inlib::sout(NAME2) << "."
              << std::endl;
          return gopaw::status_failure();
        }
        vector->multiply(*vec2);
        _sess.add_vector(NAME3,vector);
      }

    } else if(cmd_name=="VDIVIDE") {

      if(NAME3==NAME1) { //operation on itself
        // V1 / V2 = V1
        vec1->divide(*vec2);
      } else if(NAME3==NAME2) { //operation on itself
        // V1 / V2 = V2
        vec2->invert();
        vec2->multiply(*vec1);
      } else {
        if(!_sess.remove_vector(NAME3)) {
          //out << "vector_operation_C :" 
          //    << " remove " << inlib::sout(NAME3) << " failed."
          //    << std::endl;
        }
	gopaw::vector_t* vector = new gopaw::vector_t(*vec1);
        if(!vector) {
          out << "vector_operation_C :" 
              << " can't divide vector " 
              << inlib::sout(NAME1) << " with "
              << inlib::sout(NAME2) << "."
              << std::endl;
          return gopaw::status_failure();
        }
        vector->divide(*vec2);
        _sess.add_vector(NAME3,vector);
      }

    }

  }

  return gopaw::status_success();
}

int vector_print_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) {
    out << "vector_print_C : execute." << std::endl;
  }

  std::string VNAME = ku_gets();
  int DENSE = ku_geti();

  std::string NAME;
  std::vector< std::pair<int,int> > dims;
  if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return gopaw::status_failure();
  inlib::touppercase(NAME);
  gopaw::vector_t* vector = 0;
  if(!_sess.find_vector(NAME,vector)) {
    out << "vector_print_C : can't find vector " << inlib::sout(NAME) << "." << std::endl;
    return gopaw::status_failure();
  }

  if(!inlib::array_print(out,*vector,dims,NAME,DENSE)) {
    out << "vector_print_C : mismatch dimension in " << inlib::sout(VNAME) << std::endl;
  }

  return gopaw::status_success();
}

int vector_list_C(void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  if(_sess.verbose_level()) {
    out << "vector_list_C : execute." << std::endl;
  }

  std::vector<gopaw::named_vector>& vecs = _sess.named_vectors();
  unsigned int objn = vecs.size();
  std::string nameh = " Vector Name                          ";
  out << " " << nameh << "Type Length    Dim-1    Dim-2    Dim-3" << std::endl;
  int vecn = 0;
  for(unsigned int index = 0;index<objn;index++) {
    std::string& name = vecs[index].first;
    //printf("debug : %s %s\n",name.c_str(),type.c_str());
    gopaw::vector_t* vector = vecs[index].second;    
    if(vector) {
      std::string s;
      out << " ";

      s = name;
      inlib::justify(s,nameh.size(),inlib::side_left);
      out << s;

      s = "R";
      inlib::justify(s,4,inlib::side_right);
      out << s;
      out << " ";

      inlib::sprintf(s,32,"%d",vector->size());
      inlib::justify(s,6,inlib::side_right);
      out << s;
      out << "    ";

      if(vector->dimension()>=1) {
        inlib::sprintf(s,32,"%d",vector->orders()[0]);
        inlib::justify(s,5,inlib::side_right);
        out << s;
        out << "    ";
      }
      if(vector->dimension()>=2) {
        inlib::sprintf(s,32,"%d",vector->orders()[1]);
        inlib::justify(s,5,inlib::side_right);
        out << s;
        out << "    ";
      }
      if(vector->dimension()>=3) {
        inlib::sprintf(s,32,"%d",vector->orders()[2]);
        inlib::justify(s,5,inlib::side_right);
        out << s;
        out << "    ";
      }
      out << std::endl;
      vecn++;
    }
  }
  out << " Total of " << vecn << " Vector(s)" << std::endl;
 
  return gopaw::status_success();
}

int vector_find_C(const char* aName,void* a_tag){
  // The return value should match KUIP/ku_vtype comment :
  // return vector type:
  //  0 : vector name not defined
  //  1 : real vector
  //  2 : integer vector
  //
  gopaw::session& _sess = *((gopaw::session*)a_tag);
  
  inlib::params<std::string,gopaw::value_t>& variables = _sess.variables();

  std::string name = aName;
  if(name=="LOOP") return 0; //Failure

  if(variables.is_key(name)) {
    return 1; //Success
  } else { //Search also in vectors.

    // We pass here when doing :
    //    VEC/CRE v(2) R 0. 0.
    //    x = v(2)
    inlib::touppercase(name);
    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(name,vector)) {
      // If uncommenting the below, opaw_rfabbri.kumac issues the below message.
      //std::ostream& out = _sess.out();
      //out << "vector_find_C :"
      //    << " " << inlib::sout(name) << " not found."
      //    << std::endl;
      return 0; //Failure
    }
    return 1; //Success return real type.

  }
}

int vector_value_C(const char* aName,double* aValue,void* a_tag){
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  // Look if aName is of the form <name>(<index>)
  std::string name;
  int aindex = 0;
  if(!inlib::parse_array(aName,name,aindex)) {
    out << "vector_value_C :"
        << " syntax error in " << inlib::sout(std::string(aName))
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  }

  inlib::params<std::string,gopaw::value_t>& variables = _sess.variables();

  if(variables.is_key(name)) {
    gopaw::value_t var = variables.value(name);
    if(var.type()==gopaw::value_t::ARRAY_DOUBLE) {
      if(aindex==inlib::not_found()) {
        out << "vector_value_C :"
            << " vector variable without an index in " 
            << inlib::sout(std::string(aName))
            << std::endl;
        *aValue = 0;
        return 1; //Failure
      } 

      aindex--; //CERN/PAW = FORTRAN !
      std::vector<unsigned int> is;
      const std::vector<double>& stdv = var.get_array_double(is);
      if(aindex>=(int)stdv.size()) {
        out << "vector_value_C :"
            << " bad index in " << inlib::sout(std::string(aName))
            << ". Size of vector is " << (unsigned int)stdv.size()
            << std::endl;
        *aValue = 0;
        return 1; //Failure
      }
      *aValue = stdv[aindex];
      return 0; //Success

    } 

    double d;
    if(!gopaw::value_t::to_double(var,d)) { 
      out << "vector_value_C :"
          << " can't convert gopaw::value_t to a double in " 
          << inlib::sout(std::string(aName))
          << std::endl;
      *aValue = 0;
      return 1; //Failure
    }
    *aValue = d;
    return 0; //Success
  } 

  //Search also in vectors.
  // We pass here when doing :
  //    VEC/CRE v(2) R 0. 0.
  //    x = v(2)
  inlib::touppercase(name);
  gopaw::vector_t* vector = 0;
  if(!_sess.find_vector(name,vector)) {
    out << "vector_value_C :"
        << " " << inlib::sout(name) << " not found."
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  }
  if(aindex==inlib::not_found()) {
    out << "vector_value_C :"
        << " vector variable without an index in " 
        << inlib::sout(std::string(aName))
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  } 

  //FIXME : handle more dimensions.
  if(vector->dimension()!=1) {
    out << "vector_value_C :"
        << " vector dimension > 1 not treated yet in " 
        << inlib::sout(std::string(aName))
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  }
  aindex--; //CERN/PAW = FORTRAN !
  if(aindex>=(int)vector->size()) {
    out << "vector_value_C :"
        << " bad index in " << inlib::sout(std::string(aName))
        << ". Size of vector is " << vector->size()
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  }
  std::vector<unsigned int> is;
  is.push_back(aindex);
  double v;
  if(!vector->value(is,v)) {
    out << "vector_value_C :"
        << " can't get value for vector in " 
        << inlib::sout(std::string(aName))
        << std::endl;
    *aValue = 0;
    return 1; //Failure
  }
  *aValue = v;
  return 0; //Success
}
