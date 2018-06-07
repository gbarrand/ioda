#ifndef STRUTILXX_H_SEEN
#define STRUTILXX_H_SEEN

#include "machdefs.h"
#include <vector>
#include <string>

void FillVStringFrString(string const & s,vector<string>& vs,char sep = ' ');
void SplitStringToVString(string const & s,vector<string>& vs,char separator=' ');

#endif
