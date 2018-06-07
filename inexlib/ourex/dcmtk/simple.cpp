
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

int main(int argc,char** argv)
{
  if(argc<=1) {
    std::cout << "give a dcm file name." << std::endl;
    return 0;
  }
  DcmFileFormat fileformat;
  OFCondition status = fileformat.loadFile(argv[1]);
  if(status.good()) {
    OFString patientName;
    if(fileformat.getDataset()->findAndGetOFString
         (DCM_PatientName,patientName).good()) {
      std::cout << "Patient's Name: " << patientName << std::endl;
    } else {
      std::cerr << "Error: cannot access Patient's Name!" << std::endl;
    }
  } else {
    std::cerr << "Error: cannot read DICOM file (" << status.text() << ")"
              << std::endl;
  }
  return 0;
}
