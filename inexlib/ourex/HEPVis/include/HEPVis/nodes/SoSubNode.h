#ifndef HEPVIS_SOSUBNODE_H
#define HEPVIS_SOSUBNODE_H

#include <vector>

#define HEPVIS_SET_FIELDS_DEFAULT(aValue) \
    SoFieldList hepvis_fl;\
    int hepvis_fn = getFields(hepvis_fl);\
    std::vector<SbBool> hepvis_fd(hepvis_fn);\
   {for (int i=0;i<hepvis_fn;i++) {\
      SoField* field = hepvis_fl[i];\
      hepvis_fd[i] = field->isDefault();\
      field->setDefault(aValue); /* enforce writing of fields :*/\
    }}

#define HEPVIS_RESTORE_FIELDS_DEFAULT \
   {for (int i=0;i<hepvis_fn;i++) {\
      hepvis_fl[i]->setDefault(hepvis_fd[i]);\
    }}


#endif

