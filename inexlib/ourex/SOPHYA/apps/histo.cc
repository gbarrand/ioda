
#include <histos.h>
#include <randr48.h>

#include <iostream>
#include <cstdlib>

int main(int,char**) {

  SOPHYA::Histo h(-5.0,5.0,100);

  SOPHYA::ThSDR48RandGen r;

  for(unsigned int index=0;index<10000;index++) h.Add(r.Gaussian(),1);

  return EXIT_SUCCESS;
}
