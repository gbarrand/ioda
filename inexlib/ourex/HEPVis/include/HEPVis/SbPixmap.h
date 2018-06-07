#ifndef HEPVis_SbPixmap_h
#define HEPVis_SbPixmap_h

//
// Some inline helpers to manipulate bitmap / pixmaps.
//

namespace SbPixmap {

  inline unsigned char* figure2bitmap(unsigned int aW,unsigned int aH,unsigned char aFigure[]){
    // Convert a figure into a bitmap ready for glBitmap. Used in SoMarkerSet.
    // 
    // For example : 
    // static unsigned char triangle_up_line_5_5[] = { 
    //   "xxxxx"
    //   " x x "
    //   " x x "
    //   "  x  "
    //   "  x  "
    // };
    //   //OpenGL will draw with y up.
    //   Y
    //   ^    x
    //   |    x
    //   |   x x
    //   |   x x
    //   |  xxxxx
    //   O------------>X
    //
    // unsigned int w = h = 5;
    // float xorig = yorig = 2;
    // GLubyte* bitmap = (GLubyte*)SbPixmap::figure2bitmap(5,5,plus_5_5);
    // glBitmap(w,h,xorig,yorig,0.,0.,bitmap);
    // delete [] bitmap;
    int index = 0;
    unsigned char* bitmap = new unsigned char[aW * aH + 1];
    int ichar = 0;
    int ibit = 0;
    unsigned char byte = 0;
    for ( unsigned int row = 0; row < aH; row++ ){
      for ( unsigned int col = 0; col < aW; col++){ 
        unsigned char c = aFigure[ichar];
        ichar++;
        if(c==' ') {
          ibit++;
        } else { 
          byte += (1<<(7-ibit));
          ibit++;
        }
        if(ibit==8) {
          //unsigned char h = byte / 16;
          //unsigned char l = byte % 16;
          //printf("0x%x%x\n",h,l);
          bitmap[index] = byte;
          index++;
          ibit = 0;
          byte = 0;
        }
      }
      if((0<ibit)&&(ibit<8)) { // Pending byte.
        //unsigned char h = byte / 16;
        //unsigned char l = byte % 16;
        //printf("0x%x%x\n",h,l);
        bitmap[index] = byte;
        index++;
        ibit = 0;
        byte = 0;
      }
    }
    return bitmap; 
  }

  inline unsigned char* rotateBitmap(unsigned int aWidth,unsigned int aHeight,unsigned int aCols,unsigned char aBitmap[]){
    // Have a rotated (+90 degree) copy.
    //   Y
    //   ^ 
    //   | 5678  row=1
    //   | 1234  row=0
    //   O------->X
    // is transformed in :
    //   |84
    //   |73
    //   |62
    //   |51
    //   O------->X
    unsigned int bwidth = aHeight;
    unsigned int bheight = aWidth;
    unsigned int bsize = bwidth * bheight;
    if(!bsize) return 0;
    // Have an uncompressed rotated copy 
    // with 1 byte per 1 bit of the original bitmap.
    unsigned char* bptr = new unsigned char[bsize];
    //::memset(bptr,0,bsize);
    unsigned char* pfrom = aBitmap;
    for(unsigned int irow=0;irow<aHeight;irow++) {
      unsigned int icol = 0;
      for(unsigned int ibyte=0;ibyte<aCols;ibyte++) {
        unsigned char byte = *pfrom; //Contains 8 pixels of 1 bit.
        pfrom++;
        unsigned int bicol = aHeight-irow-1; // To have + 90 deg.
        for(unsigned int u=0;u<8;u++) {
          unsigned int birow = icol;
          if(icol<aWidth) {
            *(bptr+birow*bwidth+bicol) = (byte >> (7-u)) & 0x1;
          }
          icol++;
        }
      }
    }
    // Compress into a new bitmap :
    unsigned int bcols = (bwidth + 7)/8;  
    bsize = bheight * bcols;
    unsigned char* bptr2 = 0;
    if(bsize) {
      bptr2 = new unsigned char[bsize];
      //::memset(bptr2,0,bsize);
      unsigned char* ptr = bptr;
      unsigned char* ptr2 = bptr2;
      for(unsigned int irow=0;irow<bheight;irow++) {
        unsigned int icol = 0;
        for(unsigned int ibyte=0;ibyte<bcols;ibyte++) {
          unsigned char byte = 0;
          for(unsigned int u=0;u<8;u++) {
            if(icol<bwidth) { 
              byte = byte | ((*ptr) << (7-u));
              ptr++;
              icol++;
            }
          }
          *ptr2 = byte;
          ptr2++;
        }
      }
    } 
    delete [] bptr;
    return bptr2;
  }

  inline unsigned char* createBarBitmap(unsigned int aWidth,unsigned int aHeight) {
    // Create a bitmap to handle a "bar" to be put on top of a character
    // (to handle common particle physic notations).
    // The bar will be put at <aHeight> with a width of <aWidth>.
    unsigned int bytes = (aWidth+7)/8; //bytes per line.  
    unsigned int size = aHeight * bytes;
    if(!size) return 0;
    unsigned char* bitmap = new unsigned char[size];
    //::memset(bitmap,0,size);
    unsigned int i_bitmap = 0;
    int ibit = 0;
    unsigned char byte = 0;
    for(unsigned int row=0;row<aHeight;row++) {
      for(unsigned int col=0;col<aWidth;col++) {
        if(row==(aHeight-1)) byte += (1<<(7-ibit));
        ibit++;
        if(ibit==8) {
          if(i_bitmap<size) {
            bitmap[i_bitmap] = byte;
            i_bitmap++;
          }
          ibit = 0;
          byte = 0;
        }
      }
      if((0<ibit)&&(ibit<8)) { // Pending byte.
        if(i_bitmap<size) {
          bitmap[i_bitmap] = byte;
          i_bitmap++;
        }
        ibit = 0;
        byte = 0;
      }
    }
    return bitmap;
  }

  inline unsigned char* rotatePixmap(unsigned int aWidth,unsigned int aHeight,unsigned char aPixmap[]){
    // Assume a pixmap with 4 bytes per pixels.
    // Have a rotated (+90 degree) copy.
    //   Y
    //   ^ 
    //   | 5678  row=1
    //   | 1234  row=0
    //   O------->X
    // is transformed in :
    //   |84
    //   |73
    //   |62
    //   |51
    //   O------->X
    unsigned int width = aHeight;
    unsigned int height = aWidth;
    unsigned int size = width * height;
    if(!size) return 0;
    int* pixmap = new int[size];
    //::memset(pixmap,0,size);
    // assuming sizeof(int) == 4 ...        
    int* p_aPixmap = (int*)aPixmap;
    for(unsigned int arow=0;arow<aHeight;arow++) {
      for(unsigned int acol=0;acol<aWidth;acol++) {
        unsigned int col = aHeight-arow-1; // To have + 90 deg.
        unsigned int row = acol;
        *(pixmap+row*width+col) = *p_aPixmap;
        p_aPixmap++;
      }
    }
    return (unsigned char*)pixmap;
  }


  //////////////////////////////////////////////////////////////////////
  /// return data ready for glDrawPixels ///////////////////////////////
  //////////////////////////////////////////////////////////////////////

  inline float* GIFToGL(unsigned int aWidth,unsigned int aHeight,unsigned char aData[],int aColorNumber,double aReds[],double aGreens[],double aBlues[],int aTransparent) {
    // Convert a pixmap from GIF Format to OpenGL format.
    unsigned int size = 4 * aWidth * aHeight;
    if(!size) return 0;
    float* pixels = new float[size];
    if(!pixels) return 0;
    float* pPixels = pixels;
    unsigned char* pData = aData;
    unsigned char value;
    int row,col;
    for(row=(int)(aHeight-1);row>=0;row--){
      pData = aData + row * aWidth;
      for(col=0;col<(int)aWidth;col++){
        value = *pData;
        if(value<aColorNumber) {
          *pPixels = (float)aReds[value];pPixels++;
          *pPixels = (float)aGreens[value];pPixels++;
          *pPixels = (float)aBlues[value];pPixels++;
          // get the alpha channel
          if (aTransparent==value) {
            *pPixels = (float)0;pPixels++;
          } else {
            *pPixels = (float)1;pPixels++;
          }
        } else {
          *pPixels = 0;pPixels++;
          *pPixels = 0;pPixels++;
          *pPixels = 0;pPixels++;
          *pPixels = 1;pPixels++;
          // get the alpha channel
        }
        pData++;
      }
    }
    return pixels;
  }

  inline float* JPEGToGL(unsigned int aWidth,unsigned int aHeight,unsigned char aData[]){
    // Convert a pixmap from JPEG Format to RGB OpenGL format.
    unsigned int size = 3 * aWidth * aHeight;
    if(!size) return 0;
    float* pixels = new float[size];
    if(!pixels) return 0;
    float* pPixels = pixels;
    unsigned char* pData = aData;
    int row,col;
    for(row=(int)(aHeight-1);row>=0;row--){
      pData = aData + row * (aWidth * 3);
      for(col=0;col<(int)aWidth;col++){
        *pPixels = ((float)*pData)/255;pPixels++;pData++;
        *pPixels = ((float)*pData)/255;pPixels++;pData++;
        *pPixels = ((float)*pData)/255;pPixels++;pData++;
      }
    }
    return pixels;
  }

  inline void getRGB(short aValue,float& aRed,float& aGreen,float& aBlue){
  /*
    static float greys[16][3] = {
      {0.,   0.,   0.},
      {0.1,  0.1,  0.1},
      {0.2,  0.2,  0.2},
      {0.3,  0.3,  0.3},
      {0.4,  0.4,  0.4},
      {0.5,  0.5,  0.5},
      {0.6,  0.6,  0.6},
      {0.65, 0.65, 0.65},
      {0.7,  0.7,  0.7},
      {0.75, 0.75, 0.75},
      {0.8,  0.8,  0.8},
      {0.85, 0.85, 0.85},
      {0.9,  0.9,  0.9},
      {0.95, 0.95, 0.95},
      {1.,   1.,   1.},
      {1.,   1.,   1.}
    };
*/
    static const float colors[16][3] = {
      {0,   0,   0},
      {0,   0,   0.75F},
      {0,   0,   0.1F},
      {0,   0.8F,1},
      {0,   1,   1},
      {0,   1,   0.75F},
      {0,   1,   0},
      {0.6F,  0.8F,  0},
      {0.8F,  0.8F,  0},
      {1,   1,   0},
      {1,   0.75F, 0},
      {1,   0.6F,  0.5F},
      {0.75F, 0,   0.75F},
      {1,   0,   1},
      {1,   0,   0},
      {1,   0,   0}
    };
    if(aValue<0) return;
    if(aValue>=16) return;
    aRed = colors[aValue][0];
    aGreen = colors[aValue][1];
    aBlue = colors[aValue][2];
  }


  inline float* FITSToGL(int aWidth,int aHeight,short aData[]) {
    // Convert a pixmap from FITS Format to OpenGL format.
    unsigned int size = 3 * aWidth * aHeight;
    if(!size) return 0;
    float* pixels = new float[size];
    if(!pixels) return 0;
    float* pPixels = pixels;
    short* pData = aData;
    float red,green,blue;
    short value;
    int row,col;
    for(row=aHeight-1;row>=0;row--){
      pData = aData + row * aWidth;
      for(col=0;col<aWidth;col++){
        value = *pData;
        getRGB(value,red,green,blue);
        *pPixels = red;pPixels++;
        *pPixels = green;pPixels++;
        *pPixels = blue;pPixels++;
        pData++;
      }
    }
    return pixels;
  }

}


#endif
