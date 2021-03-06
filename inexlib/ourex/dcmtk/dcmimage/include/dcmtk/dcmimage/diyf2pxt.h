/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomYBR422PixelTemplate (Header)
 *
 *  Last Update:      $Author: barrand $
 *  Update Date:      $Date: 2013/07/02 07:15:11 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIYF2PXT_H
#define DIYF2PXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle YCbCr Full 4:2:2 pixel data
 */
template<class T1, class T2>
class DiYBR422PixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to DICOM document
     *  @param  pixel   pointer to input pixel representation
     *  @param  status  reference to status variable
     *  @param  bits    number of bits per sample
     *  @param  rgb     flag, convert color model to RGB only if true
     */
    DiYBR422PixelTemplate(const DiDocument *docu,
                          const DiInputPixel *pixel,
                          EI_Status &status,
                          const int bits,
                          const OFBool rgb)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status, 2)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
        {
            if (this->PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                DCMIMAGE_ERROR("invalid value for 'PlanarConfiguration' (" << this->PlanarConfiguration << ")");
            }
            else
                convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), bits, rgb);
        }
    }

    /** destructor
     */
    virtual ~DiYBR422PixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  bits       number of bits per sample
     *  @param  rgb        flag, convert color model to RGB only if true
     */
    void convert(const T1 *pixel,
                 const int bits,
                 const OFBool rgb)
    {
      if (DiColorPixelTemplate<T2>::Init(pixel)) //G.Barrand
        {
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            register unsigned long i;
            register const T1 *p = pixel;
            register T2 *r = this->Data[0];
            register T2 *g = this->Data[1];
            register T2 *b = this->Data[2];
            register T2 y1;
            register T2 y2;
            register T2 cb;
            register T2 cr;
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (rgb)    /* convert to RGB model */
            {
                const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
                for (i = count / 2; i != 0; --i)
                {
                    y1 = removeSign(*(p++), offset);
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    convertValue(*(r++), *(g++), *(b++), y1, cb, cr, maxvalue);
                    convertValue(*(r++), *(g++), *(b++), y2, cb, cr, maxvalue);
                }
            } else {    /* retain YCbCr model: YCbCr_422_full -> YCbCr_full */
                for (i = count / 2; i != 0; --i)
                {
                    y1 = removeSign(*(p++), offset);
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    *(r++) = y1;
                    *(g++) = cb;
                    *(b++) = cr;
                    *(r++) = y2;
                    *(g++) = cb;
                    *(b++) = cr;
                }
            }
        }
    }

    /** convert a single YCbCr value to RGB
     */
    inline void convertValue(T2 &red,
                             T2 &green,
                             T2 &blue,
                             const T2 y,
                             const T2 cb,
                             const T2 cr,
                             const T2 maxvalue)
    {
        double dr = OFstatic_cast(double, y) + 1.4020 * OFstatic_cast(double, cr) - 0.7010 * OFstatic_cast(double, maxvalue);
        double dg = OFstatic_cast(double, y) - 0.3441 * OFstatic_cast(double, cb) - 0.7141 * OFstatic_cast(double, cr) + 0.5291 * OFstatic_cast(double, maxvalue);
        double db = OFstatic_cast(double, y) + 1.7720 * OFstatic_cast(double, cb) - 0.8859 * OFstatic_cast(double, maxvalue);
        red   = (dr < 0.0) ? 0 : (dr > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, dr);
        green = (dg < 0.0) ? 0 : (dg > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, dg);
        blue  = (db < 0.0) ? 0 : (db > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, db);
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diyf2pxt.h,v $
 * Revision 1.2  2013/07/02 07:15:11  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2013/04/16 19:23:57  barrand
 *
 *
 * Revision 1.25  2010-10-14 13:16:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.24  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.23  2009-11-25 14:31:21  joergr
 * Removed inclusion of header file "ofconsol.h".
 *
 * Revision 1.22  2009-10-14 10:25:14  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.21  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.20  2006-08-15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.19  2005/12/08 16:02:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.17  2003/12/23 12:33:01  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header. Added missing API documentation.
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.16  2002/06/26 16:20:53  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 *
 * Revision 1.15  2002/05/24 09:53:06  joergr
 * Added missing #include "ofconsol.h".
 *
 * Revision 1.14  2001/11/09 16:47:03  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.13  2001/09/28 13:55:41  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.12  2001/06/01 15:49:33  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/04/28 12:39:33  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.10  2000/04/27 13:15:16  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.9  2000/03/08 16:21:54  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/03 14:07:52  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.7  1999/09/17 14:03:47  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.6  1999/04/28 12:45:21  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/02/03 16:55:29  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.4  1999/01/20 14:48:01  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.3  1998/11/27 14:21:48  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 *
 * Revision 1.2  1998/05/11 14:53:33  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
