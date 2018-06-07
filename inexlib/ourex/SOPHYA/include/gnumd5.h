#ifndef GNUMD5_H
#define GNUMD5_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \cond */
typedef struct {
    uint_8 mem_align;     /* To force memory alignment on SunOS 22/02/2001 */
    uint_1 buf[64];
    uint_4 A,B,C,D;	  /* chaining variables */
    uint_4  nblocks;
    int  count;
} MD5_CONTEXT;
/*! \endcond */

void
md5_init( MD5_CONTEXT *ctx );

void
md5_write( MD5_CONTEXT *hd, uint_1 *inbuf, size_t inlen);

void
md5_final( MD5_CONTEXT *hd );




#ifdef __cplusplus
}
#endif



#endif
