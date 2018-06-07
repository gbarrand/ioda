#ifndef HEPVis_SbLUT_h
#define HEPVis_SbLUT_h
 
class SbLUT {
public:
  enum Type {
    IDENTITY = 0,
    LINEAR = 1,
    LOG = 2
  };
  SbLUT();
  SbLUT(int);
  ~SbLUT();
  void initialize(Type,int,int,int);
  int getCellNumber();
  unsigned char* getCells();
private:
  Type fType;
  int fMin;
  int fMax;
  int fNbin;
  int fNumber;
  unsigned char* fList;
};

#endif
