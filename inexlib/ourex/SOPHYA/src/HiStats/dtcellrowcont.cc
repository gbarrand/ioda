//-----------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2005-2013
//-----------------------------------------------------------

#include "dtcellrowcont.h"
#include <ctype.h>
#include "pexceptions.h"


namespace SOPHYA {

/*!
   \class DataTableRow
   \ingroup HiStats

   \brief Class for representing and manipulating DataTable row content.

   This class is intented to be used with datatable classes
   (inheriting from BaseDataTable) for representing a row (line)
   of the table. 
*/
DataTableRow::DataTableRow( vector<string> const& colnames, vector<size_t> const & colvsz)
{
  if ((colnames.size() < 1)||(colnames.size() != colvsz.size())) 
    throw ParmError("DataTableRow::DataTableRow(cn,cvsz) cn.size()==0 || cn.size()!=cvsz.size() ");
  size_=0;
  for(size_t k=0; k<colnames.size(); k++) {
    colseqp_.push_back(size_);
    nm2idx_[colnames[k]] = k;
    size_ += colvsz[k];
  }
  mtv_ = new MuTyV[ size_ ];
}

DataTableRow::DataTableRow(DataTableRow const & a )
  : size_(a.size_), colseqp_(a.colseqp_), nm2idx_(a.nm2idx_)
{
  mtv_ = new MuTyV[ size_ ];
  for(size_t i=0; i<size_; i++) mtv_[i]=a.mtv_[i]; 
}

DataTableRow::~DataTableRow()
{
  delete[] mtv_;
}

MuTyV* DataTableRow::getPtr(size_t k)
{
  if (k>=colseqp_.size()) {
      std::string msg = "DataTableRow::getPtr(k) out of range  " ;
    throw NotFoundExc(msg);
  }
  return (mtv_ + colseqp_[k]);
}

MuTyV* DataTableRow::getPtr(string const& colname)
{
  map<string, size_t>::const_iterator it = nm2idx_.find(colname);
  if (it == nm2idx_.end()) { 
    std::string msg = "DataTableRow::getPtr( " ;
    msg += colname;    msg += " ) Not found column name";
    throw NotFoundExc(msg);
  }
  return (mtv_ + colseqp_[(*it).second]);
}


ostream&   DataTableRow::Print(ostream& os) const
{
  for(size_t k=0; k<size_; k++) 
    os << (string)mtv_[k] << " ";
  return os;
}


/*!
   \class DataTableRowPtr
   \ingroup HiStats

   \brief Class for direct access to data table row content through pointer (access to the row memory area)  

   This class is intented to be used with datatable classes
   (inheriting from BaseDataTable) for accessing data in a row 
   of the table. It provides a more efficient way of accesing the data
   compared to the more easy to use DataTableRow class, minimizing converion 
   and memory to memory data copies.
*/

  DataTableRowPtr::DataTableRowPtr(vector< DataTableCellType > const & coltypes, vector<string> const & colnames, 
				   vector<size_t> const & colvsz)
{
  if ((coltypes.size() < 1)||(colnames.size() != coltypes.size())||(coltypes.size() != colvsz.size())) 
    throw ParmError("DataTableRowPtr::DataTableRowPtr(ct,cn,cvsz) cn.size()==0 ||ct.size()!=cn.size()||ct.size()!=cvsz.size() ");
  cellsizes_=colvsz;
  for(size_t k=0; k<colnames.size(); k++) nm2idx_[colnames[k]] = k;
  cellptr_ = new DTCell[ Size() ];
  for(size_t i=0; i<Size(); i++) cellptr_[i]=DTCell(coltypes[i],NULL,getDataSize(coltypes[i]));
}

  DataTableRowPtr::DataTableRowPtr(DataTableRowPtr const & a )
    : cellsizes_(a.cellsizes_), nm2idx_(a.nm2idx_)
{
  cellptr_ = new DTCell[ Size() ];
  for(size_t i=0; i<Size(); i++) cellptr_[i] = a.cellptr_[i];
}

  DataTableRowPtr::~DataTableRowPtr()
{
  delete[] cellptr_;
}

void DataTableRowPtr::CopyContentFrom(DataTableRowPtr const & a)
{
  for(size_t k=0; k<Size(); k++) {
    if (CellType(k) != DTC_StringType) 
      memcpy(cellptr_[k].Ptr().p, a.cellptr_[k].Ptr().p, cellsizes_[k]*cellptr_[k].DataSize());
    else for(size_t j=0; j<cellsizes_[k]; j++)
	   cellptr_[k].Ptr().s[j] = a.cellptr_[k].Ptr().s[j];
  }
}

void DataTableRowPtr::CopyContentFrom(DataTableRowPtr const & a, vector< pair<size_t, size_t> > const & cid)
{
  for(size_t i=0; i<cid.size(); i++) {
    size_t ks=cid[i].first;
    size_t kd=cid[i].second;
    //DBG    cout << " *DBG*CopyContentFrom- NCols=" << NCols() << " ks=" << ks << " kd=" << kd << " CellType(kd)=" << CellType(kd) 
    //DBG	 << " cellsizes_[kd]= " << cellsizes_[kd] << " DataSize=" << cellptr_[kd].DataSize() << endl;
    if (CellType(kd) != DTC_StringType) 
      memcpy(cellptr_[kd].Ptr().p, a.cellptr_[ks].Ptr().p, cellsizes_[kd]*cellptr_[kd].DataSize());
    else for(size_t j=0; j<cellsizes_[kd]; j++)
	   cellptr_[kd].Ptr().s[j] = a.cellptr_[ks].Ptr().s[j];
  }
}

ostream&   DataTableRowPtr::Print(ostream& os, const char* csep, const char* vsep) const
{
  
  for(size_t k=0; k<Size(); k++) {
    switch(CellType(k)) {
    case DTC_IntegerType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().i4+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_LongType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().i8+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_FloatType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().f4+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_DoubleType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().f8+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_ComplexType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().z4+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_DoubleComplexType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().z8+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_DateTimeType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().tms+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_StringType:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().s+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    case DTC_FMLStr16Type:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().s16+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
     break;
    case DTC_FMLStr64Type:
      for(size_t j=0; j<cellsizes_[k]; j++) {
 	os << *(cellptr_[k].Ptr().s64+j);
	if (j!=cellsizes_[k]-1)  os << vsep; 
      }
      break;
    default:
      break;
    }
  }
  return os;
}


size_t DataTableRowPtr::Index(string const & colname) const 
{
  map<string, size_t>::const_iterator it = nm2idx_.find(colname);
  if (it == nm2idx_.end()) { 
    std::string msg = "DataTableRowPtr::Index( " ;
    msg += colname;    msg += " ) Not found column name";
    throw NotFoundExc(msg);
  }
  return ((*it).second);
}

size_t  DataTableRowPtr::getDataSize(DataTableCellType typ)
{
  switch (typ) {
  case DTC_IntegerType:
    return sizeof(int_4);
    break;
  case DTC_LongType:
    return sizeof(int_8);
    break;
  case DTC_FloatType:
    return sizeof(r_4);
    break;
  case DTC_DoubleType:
    return sizeof(r_8);
    break;
  case DTC_ComplexType:
    return sizeof(complex<r_4>);
    break;
  case DTC_DoubleComplexType:
    return sizeof(complex<r_8>);
    break;
  case DTC_DateTimeType:
    return sizeof(TimeStamp);
    break;
  case DTC_StringType:
    return sizeof(std::string);
    break;
  case DTC_FMLStr16Type:
    return sizeof(String16);
    break;
  case DTC_FMLStr64Type:
    return sizeof(String64);
    break;
  default:
    return 0;
    break;
  }
}

} // FIN namespace SOPHYA 
