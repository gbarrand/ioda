/* from jwrgif.c
 *
 * Copyright (C) 1991, 1992, 1993, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to write output images in GIF format.
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume output to
 * an ordinary stdio stream.
 *
 * These routines are invoked via the methods put_pixel_rows, put_color_map,
 * and output_init/term.
 */

// this :
#include <HEPVis/SbJGIF.h>

#include <stdio.h>

typedef unsigned char JSAMPLE;
typedef JSAMPLE* JSAMPROW;        /* ptr to one image row of pixel samples. */
typedef JSAMPROW* JSAMPARRAY;        /* ptr to some rows (a 2-D sample array) */
typedef JSAMPARRAY* JSAMPIMAGE;        /* a 3-D sample array: top index is color */

typedef short INT16;
typedef long  INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef int boolean;
#define GETJSAMPLE(value)  (value)

typedef enum {                        /* defines known color spaces */
        CS_UNKNOWN,                /* error/unspecified */
        CS_GRAYSCALE,                /* monochrome (only 1 component) */
        CS_RGB,                        /* red/green/blue */
        CS_YCbCr,                /* Y/Cb/Cr (also known as YUV) */
        CS_YIQ,                        /* Y/I/Q */
        CS_CMYK                        /* C/M/Y/K */
} COLOR_SPACE;

#define FAR
#define JFWRITE(file,buf,sizeofbuf)  \
  ((size_t) fwrite((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
#undef FALSE                /* in case these macros already exist */
#undef TRUE
#define FALSE        0       /* values of boolean */
#define TRUE        1


static struct {
 FILE*       output_file; 
 long        image_width;
 long        image_height;
 COLOR_SPACE out_color_space;
 short       data_precision;
 boolean     quantize_colors;
} Class = {NULL,0,0,CS_RGB,8,TRUE};

#define        MAX_LZW_BITS        12        /* maximum LZW code size (4096 symbols) */
typedef INT16 code_int;                /* must hold -1 .. 2**MAX_LZW_BITS */
#define LZW_TABLE_SIZE        ((code_int) 1 << MAX_LZW_BITS)
#define HSIZE                5003        /* hash table size for 80% occupancy */
typedef int hash_int;                /* must hold -2*HSIZE..2*HSIZE */

static void output(code_int code);
static void clear_hash();
static void clear_block();
static void compress_init(int i_bits);
static void compress_byte(int c);
static void compress_term();
static void put_word(UINT16 w);
static void put_3bytes(int val);
static void emit_header(int num_colors, JSAMPARRAY colormap);
static void jzero_far(void FAR * target, size_t bytestozero);
static void ReportError(const char*);

static int n_bits;                /* current number of bits/code */
static code_int maxcode;        /* maximum code, given n_bits */
#define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)

static int init_bits;                /* initial n_bits ... restored after clear */

static code_int ClearCode;        /* clear code (doesn't change) */
static code_int EOFCode;        /* EOF code (ditto) */

static code_int FreeCode;        /* first not-yet-used symbol code */

/*
 * The LZW hash table consists of three parallel arrays:
 *   hash_code[i]        code of symbol in slot i, or 0 if empty slot
 *   hash_prefix[i]        symbol's prefix code; undefined if empty slot
 *   hash_suffix[i]        symbol's suffix character; undefined if empty slot
 * where slot values (i) range from 0 to HSIZE-1.
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / suffix character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.
 *
 * The hash tables are allocated from FAR heap space since they would use up
 * rather a lot of the near data space in a PC.
 */

static code_int FAR *hash_code;        /* => hash table of symbol codes */
static code_int FAR *hash_prefix; /* => hash table of prefix symbols */
static UINT8 FAR *hash_suffix;        /* => hash table of suffix bytes */


/*
 * Routines to package compressed data bytes into GIF data blocks.
 * A data block consists of a count byte (1..255) and that many data bytes.
 */

static int  bytesinpkt;                /* # of bytes in current packet */
static char packetbuf[256];        /* workspace for accumulating packet */
//////////////////////////////////////////////////////////////////////////////
void SbJGIF::beginGIF_Stream(
 void* a_file
,long a_width
,long a_height
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  Class.output_file     = (FILE*)a_file;
  Class.image_width     = a_width;
  Class.image_height    = a_height;

  /* Allocate space for hash table */
  hash_code = (code_int FAR*)new code_int[HSIZE];
  hash_prefix = (code_int FAR*)new code_int[HSIZE];
  hash_suffix = (UINT8 FAR*)new UINT8[HSIZE];
  /*
   * If we aren't quantizing, put_color_map won't be called,
   * so emit the header now.  This only happens with gray scale output.
   * (If we are quantizing, wait for the color map to be provided.)
   */
  if (Class.quantize_colors==FALSE) emit_header(256, (JSAMPARRAY) NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SbJGIF::putColormapInGIF_Stream(
 int num_colors
,JSAMPARRAY colormap
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  emit_header(num_colors, colormap);
}
//////////////////////////////////////////////////////////////////////////////
void SbJGIF::putImageInGIF_Stream(
 long a_width
,int num_rows
,JSAMPIMAGE pixel_data
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  for (int row = 0; row < num_rows; row++) {
    JSAMPROW ptr = pixel_data[0][row];
    for (int col = a_width; col > 0; col--) {
      compress_byte(GETJSAMPLE(*ptr));
      ptr++;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void SbJGIF::endGIF_Stream(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  /* Flush LZW mechanism */
  compress_term();
  /* Write a zero-length data block to end the series */
  putc(0, Class.output_file);
  /* Write the GIF terminator mark */
  putc(';', Class.output_file);
  /* Make sure we wrote the output file OK */
  fflush(Class.output_file);
  if (ferror(Class.output_file)!=0) 
    ReportError("SbJGIF::endGIF_Stream : fflush failed");

  /* Free space */
  delete [] hash_code;
  delete [] hash_prefix;
  delete [] hash_suffix;
}
//////////////////////////////////////////////////////////////////////////////
static void flush_packet(
)
//////////////////////////////////////////////////////////////////////////////
// flush any accumulated data
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (bytesinpkt > 0) {                /* never write zero-length packet */
    packetbuf[0] = (char) bytesinpkt++;
    if (JFWRITE(Class.output_file, packetbuf, bytesinpkt) 
        != (size_t) bytesinpkt) 
      ReportError("SbJGIF::endGIF_Stream : JFWRITE failed");
    bytesinpkt = 0;
  }
}

/* Add a character to current packet; flush to disk if necessary */
#define CHAR_OUT(c)  \
        { packetbuf[++bytesinpkt] = (char) (c);  \
            if (bytesinpkt >= 255)  \
              flush_packet();  \
        }

/* Routine to convert variable-width codes into a byte stream */
static INT32 cur_accum;                /* holds bits not yet output */
static int cur_bits;                /* # of bits in cur_accum */
//////////////////////////////////////////////////////////////////////////////
void output(
 code_int code
)
//////////////////////////////////////////////////////////////////////////////
// Emit a code of n_bits bits
// Uses cur_accum and cur_bits to reblock into 8-bit bytes
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  cur_accum |= ((INT32) code) << cur_bits;
  cur_bits += n_bits;

  while (cur_bits >= 8) {
    CHAR_OUT(cur_accum & 0xFF)
    cur_accum >>= 8;
    cur_bits -= 8;
  }

  /*
   * If the next entry is going to be too big for the code size,
   * then increase it, if possible.  We do this here to ensure
   * that it's done in sync with the decoder's codesize increases.
   */
  if (FreeCode > maxcode) {
    n_bits++;
    if (n_bits == MAX_LZW_BITS)
      maxcode = LZW_TABLE_SIZE;        /* FreeCode will never exceed this */
    else
      maxcode = MAXCODE(n_bits);
  }
}

/* The LZW algorithm proper */
static code_int waiting_code;        /* symbol not yet output; may be extendable */
static boolean  first_byte;        /* if TRUE, waiting_code is not valid */
//////////////////////////////////////////////////////////////////////////////
void clear_hash(
)
//////////////////////////////////////////////////////////////////////////////
// Fill the hash table with empty entries
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  /* It's sufficient to zero hash_code[] */
  jzero_far((void FAR *) hash_code, HSIZE * sizeof(code_int));
}
//////////////////////////////////////////////////////////////////////////////
void clear_block(
)
//////////////////////////////////////////////////////////////////////////////
// Reset compressor and issue a Clear code
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  clear_hash();                        /* delete all the symbols */
  FreeCode = ClearCode + 2;
  output(ClearCode);                /* inform decoder */
  n_bits = init_bits;                /* reset code size */
  maxcode = MAXCODE(n_bits);
}
//////////////////////////////////////////////////////////////////////////////
void compress_init(
 int i_bits
)
//////////////////////////////////////////////////////////////////////////////
// Initialize LZW compressor
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  /* init all the static variables */
  n_bits = init_bits = i_bits;
  maxcode = MAXCODE(n_bits);
  ClearCode = ((code_int) 1 << (init_bits - 1));
  EOFCode = ClearCode + 1;
  FreeCode = ClearCode + 2;
  first_byte = TRUE;                /* no waiting symbol yet */
  /* init output buffering vars */
  bytesinpkt = 0;
  cur_accum = 0;
  cur_bits = 0;
  /* clear hash table */
  clear_hash();
  /* GIF specifies an initial Clear code */
  output(ClearCode);
}
//////////////////////////////////////////////////////////////////////////////
void compress_byte(
 int c
)
//////////////////////////////////////////////////////////////////////////////
// Accept and compress one 8-bit byte
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
   hash_int i;
   hash_int disp;

  if (first_byte==TRUE) {                /* need to initialize waiting_code */
    waiting_code = c;
    first_byte = FALSE;
    return;
  }

  /* Probe hash table to see if a symbol exists for
   * waiting_code followed by c.
   * If so, replace waiting_code by that symbol and return.
   */
  i = ((hash_int) c << (MAX_LZW_BITS-8)) + waiting_code;
  /* i is less than twice 2**MAX_LZW_BITS, therefore less than twice HSIZE */
  if (i >= HSIZE)
    i -= HSIZE;
  
  if (hash_code[i] != 0) {        /* is first probed slot empty? */
    if (hash_prefix[i] == waiting_code && hash_suffix[i] == (UINT8) c) {
      waiting_code = hash_code[i];
      return;
    }
    if (i == 0)                        /* secondary hash (after G. Knott) */
      disp = 1;
    else
      disp = HSIZE - i;
    while (1) {
      i -= disp;
      if (i < 0)
        i += HSIZE;
      if (hash_code[i] == 0)
        break;                        /* hit empty slot */
      if (hash_prefix[i] == waiting_code && hash_suffix[i] == (UINT8) c) {
        waiting_code = hash_code[i];
        return;
      }
    }
  }

  /* here when hashtable[i] is an empty slot; desired symbol not in table */
  output(waiting_code);
  if (FreeCode < LZW_TABLE_SIZE) {
    hash_code[i] = FreeCode++;        /* add symbol to hashtable */
    hash_prefix[i] = waiting_code;
    hash_suffix[i] = (UINT8) c;
  } else
    clear_block();
  waiting_code = c;
}
//////////////////////////////////////////////////////////////////////////////
void compress_term(
)
//////////////////////////////////////////////////////////////////////////////
// Clean up at end
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  /* Flush out the buffered code */
  if (first_byte==FALSE)
    output(waiting_code);
  /* Send an EOF code */
  output(EOFCode);
  /* Flush the bit-packing buffer */
  if (cur_bits > 0) {
    CHAR_OUT(cur_accum & 0xFF)
  }
  /* Flush the packet buffer */
  flush_packet();
}
/* GIF header construction */
//////////////////////////////////////////////////////////////////////////////
void put_word(
 UINT16 w
)
//////////////////////////////////////////////////////////////////////////////
// Emit a 16-bit word, LSB first
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  putc(w & 0xFF, Class.output_file);
  putc((w >> 8) & 0xFF,Class.output_file);
}
//////////////////////////////////////////////////////////////////////////////
void put_3bytes(
 int val
)
//////////////////////////////////////////////////////////////////////////////
// Emit 3 copies of same byte value --- handy subr for colormap construction
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  putc(val, Class.output_file);
  putc(val, Class.output_file);
  putc(val, Class.output_file);
}
//////////////////////////////////////////////////////////////////////////////
void emit_header(
 int num_colors
,JSAMPARRAY colormap
)
//////////////////////////////////////////////////////////////////////////////
// Output the GIF file header, including color map
// If colormap==NULL, synthesize a gray-scale colormap
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int BitsPerPixel, ColorMapSize, InitCodeSize, FlagByte;
  int cshift = Class.data_precision - 8;
  int i;

  if (num_colors > 256) ReportError ("emit_header : bad value");
  /* Compute bits/pixel and related values */
  BitsPerPixel = 1;
  while (num_colors > (1 << BitsPerPixel))
    BitsPerPixel++;
  ColorMapSize = 1 << BitsPerPixel;
  if (BitsPerPixel <= 1)
    InitCodeSize = 2;
  else
    InitCodeSize = BitsPerPixel;
  /*
   * Write the GIF header.
   * Note that we generate a plain GIF87 header for maximum compatibility.
   */
  putc('G', Class.output_file);
  putc('I', Class.output_file);
  putc('F', Class.output_file);
  putc('8', Class.output_file);
  putc('7', Class.output_file);
  putc('a', Class.output_file);
  /* Write the Logical Screen Descriptor */
  put_word((UINT16) Class.image_width);
  put_word((UINT16) Class.image_height);
  FlagByte = 0x80;                /* Yes, there is a global color table */
  FlagByte |= (BitsPerPixel-1) << 4; /* color resolution */
  FlagByte |= (BitsPerPixel-1);        /* size of global color table */
  putc(FlagByte, Class.output_file);
  putc(0, Class.output_file);        /* Background color index */
  putc(0, Class.output_file);        /* Reserved in GIF87 (aspect ratio in GIF89) */
  /* Write the _Class Color Map */
  /* If the color map is more than 8 bits precision, */
  /* we reduce it to 8 bits by shifting */
  for (i=0; i < ColorMapSize; i++) {
    if (i < num_colors) {
      if (colormap != NULL) {
        if (Class.out_color_space == CS_RGB) {
          /* Normal case: RGB color map */
          putc(GETJSAMPLE(colormap[0][i]) >> cshift, Class.output_file);
          putc(GETJSAMPLE(colormap[1][i]) >> cshift, Class.output_file);
          putc(GETJSAMPLE(colormap[2][i]) >> cshift, Class.output_file);
        } else {
          /* Grayscale "color map": possible if quantizing grayscale image */
          put_3bytes(GETJSAMPLE(colormap[0][i]) >> cshift);
        }
      } else {
        /* Create a gray-scale map of num_colors values, range 0..255 */
        put_3bytes((i * 255 + (num_colors-1)/2) / (num_colors-1));
      }
    } else {
      /* fill out the map to a power of 2 */
      put_3bytes(0);
    }
  }
  /* Write image separator and Image Descriptor */
  putc(',', Class.output_file); /* separator */
  put_word((UINT16) 0);                /* left/top offset */
  put_word((UINT16) 0);
  put_word((UINT16) Class.image_width); /* image size */
  put_word((UINT16) Class.image_height);
  /* flag byte: not interlaced, no local color map */
  putc(0x00, Class.output_file);
  /* Write Initial Code Size byte */
  putc(InitCodeSize, Class.output_file);

  /* Initialize for LZW compression of image data */
  compress_init(InitCodeSize+1);
}
//////////////////////////////////////////////////////////////////////////////
void jzero_far(
 void FAR* target
,size_t bytestozero
)
//////////////////////////////////////////////////////////////////////////////
// Zero out a chunk of FAR memory.
// This might be sample-array data, block-array data, or alloc_medium data.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
   char FAR * ptr = (char FAR *) target;
   size_t count;

  for (count = bytestozero; count > 0; count--) {
    *ptr++ = 0;
  }
}
//////////////////////////////////////////////////////////////////////////////
void ReportError (
 const char* a_name
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  printf("%s\n",a_name);
}
