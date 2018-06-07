// this :
#include <HEPVis/nodes/SoFile.h>

#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbFile.h>
#include <HEPVis/SbString.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

SO_NODE_SOURCE(HEPVis_SoFile)

HEPVis_SoFile::CurlFunction HEPVis_SoFile::fCurlFunction = 0; //FIXME
void HEPVis_SoFile::setCurlFunction(CurlFunction aFunc) { //FIXME : rm
  fCurlFunction = aFunc;
}


void HEPVis_SoFile::initClass() {
  SO_NODE_INIT_CLASS(HEPVis_SoFile,SoFile,"File");
}
HEPVis_SoFile::HEPVis_SoFile() {
  SO_NODE_CONSTRUCTOR(HEPVis_SoFile);
}
HEPVis_SoFile::~HEPVis_SoFile(){}

#if((COIN_MAJOR_VERSION>=2)&&(COIN_MINOR_VERSION>=5))
#else
void HEPVis_SoFile::search(SoSearchAction* action) {
  SoFile::doAction((SoAction*)action);
}
#endif

//#include <cstdio> //tmpnam

SbBool HEPVis_SoFile::readNamedFile(SoInput* in) {
  SbString local = "";
  SbString s = name.getValue();
  SbString file;
  if(s.getSubString(0,4)=="http:") {
    // build a local name :
    //local  = ::tmpnam(NULL); //Linux : warning.
    if(!SbFile::tmpname(local,"","SoCurl_","")) {
      SoDebugError::postInfo("SoCurl::readNamedFile",
        "Can't build a tmpname.");
      return FALSE;
    }
    HEPVis_SoFile::CurlFunction curl_func = 
      fCurlFunction?fCurlFunction:curl_file;
    if(curl_func(s,local)==FALSE) { 
      SoDebugError::postInfo("HEPVis_SoFile::readNamedFile",
        "Can't get URL \"%s\".",s.getString());
      SbFile::remove(local);
      return FALSE;
    } else {
      file = local;
    }
  } else {
    file = SbFile::getFullName(s);
  } 
 {SbBool flag = name.enableNotify(FALSE);
  name.setValue(file); //do we definitly replace name ?
  name.enableNotify(flag);}
  SbBool status = SoFile::readNamedFile(in);
 {SbBool flag = name.enableNotify(FALSE);
  name.setValue(s); //restore original name (in case we SoWrite the node).
  name.enableNotify(flag);}
  if(local.getLength()>0) SbFile::remove(local);
  return status;
}

#include <cstdlib> //::system

// simply call ::system("curl") :
SbBool HEPVis_SoFile::curl_file(const SbString& a_url,const SbString& a_out_file) {
#if TARGET_OS_IPHONE
  SoDebugError::postInfo("HEPVis_SoFile::cur_file","::system() not available on iOS.");
  return FALSE;
#else
  SbString cmd("curl ");
  cmd += " --silent ";
  cmd += " --compressed ";
  cmd += a_url;
  cmd += " --output ";
  cmd += a_out_file;
  if(::system(cmd.getString())==(-1)) {
    SoDebugError::postInfo("HEPVis_SoFile::cur_file",
      "call to ::system(\"curl <options>\") failed.");
    SbFile::remove(a_out_file);
    return FALSE;
  }

  if(SbFile::exists(a_out_file)==FALSE) {
    SoDebugError::postInfo("HEPVis_SoFile::cur_file",
			   "can't get \"%s\".",a_url.getString());
    return FALSE;
  }

  return TRUE;
#endif //TARGET_OS_IPHONE
}

/*
#ifdef HEPVIS_HAS_CURL
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string>

class File {
public:
  File(const SbString& a_name):m_name(a_name),m_FILE(0){}
public:
  SbString m_name;
  FILE* m_FILE;
};

static int my_fwrite(
 void* a_buffer
,size_t a_size
,size_t a_nmemb
,void* a_tag
)
{
  File* out = (File*)a_tag;

  if(out && !out->m_FILE) {
    // open file for writing :
    out->m_FILE = ::fopen(out->m_name.getString(),"wb");
    if(!out->m_FILE) return -1; //
  }
  return ::fwrite(a_buffer,a_size,a_nmemb,out->m_FILE);
}


SbBool HEPVis_SoFile::curl_file(
 const SbString& a_url
,const SbString& a_out_file
) {
  File ftpfile(a_out_file);

  curl_global_init(CURL_GLOBAL_DEFAULT);

  SbBool status = FALSE;

  CURL* curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl,CURLOPT_URL,a_url.getString());
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,my_fwrite);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ftpfile);

    //curl_easy_setopt(curl,CURLOPT_VERBOSE,1);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if(ftpfile.m_FILE) ::fclose(ftpfile.m_FILE);

    status = (res!=CURLE_OK?FALSE:TRUE);
  }

  curl_global_cleanup();

  if(status==FALSE) SbFile::remove(a_out_file);

  return status;
}

#else
SbBool curl_file(const SbString&,const SbString&) { 
  static bool print_msg = false;
  if(!print_msg) {
    SoDebugError::postInfo("HEPVis_SoFile::cur_file",
      "HEPVisUtils not reconstructed with HEPVIS_HAS_CURL.");
    print_msg = true;
  }
  return FALSE;
}
#endif
*/

