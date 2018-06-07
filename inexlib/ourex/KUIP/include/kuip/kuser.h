#ifndef kuip_kuser_h
#define kuip_kuser_h
/*
 * $Id: kuser.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: kuser.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.22  2009/06/30 12:26:52  barrand
 * *** empty log message ***
 *
 * Revision 1.21  2009/06/30 08:48:58  barrand
 * *** empty log message ***
 *
 * Revision 1.20  2009/06/29 16:27:16  barrand
 * *** empty log message ***
 *
 * Revision 1.19  2009/06/17 09:28:12  barrand
 * *** empty log message ***
 *
 * Revision 1.18  2009/06/04 14:17:17  barrand
 * *** empty log message ***
 *
 * Revision 1.17  2009/06/04 12:50:12  barrand
 * *** empty log message ***
 *
 * Revision 1.16  2005/09/19 07:14:44  barrand
 * *** empty log message ***
 *
 * Revision 1.15  2005/06/08 07:55:10  barrand
 * *** empty log message ***
 *
 * Revision 1.14  2004/12/23 14:53:01  barrand
 * *** empty log message ***
 *
 * Revision 1.13  2004/09/04 09:40:59  barrand
 * *** empty log message ***
 *
 * Revision 1.12  2004/09/03 14:36:13  barrand
 * *** empty log message ***
 *
 * Revision 1.11  2004/09/03 14:33:47  barrand
 * *** empty log message ***
 *
 * Revision 1.10  2004/08/01 14:43:23  barrand
 * *** empty log message ***
 *
 * Revision 1.9  2004/08/01 14:23:06  barrand
 * *** empty log message ***
 *
 * Revision 1.8  2004/07/14 10:04:02  barrand
 * *** empty log message ***
 *
 * Revision 1.7  2003/09/10 09:27:32  barrand
 * *** empty log message ***
 *
 * Revision 1.6  2003/09/10 07:17:45  barrand
 * *** empty log message ***
 *
 * Revision 1.5  2000/05/28 04:19:40  barrand
 * *** empty log message ***
 *
 * Revision 1.4  1999/12/10  08:34:39  barrand
 * *** empty log message ***
 *
 * Revision 1.3  1999/06/22  15:29:40  barrand
 * *** empty log message ***
 *
 * Revision 1.2  1999/06/22  10:44:01  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25  13:17:45  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */

#include <stddef.h> /*GB*/
#include <time.h>   /*GB*/

#define KUMAC_UNWIND -30041961 /* error status to quit macro execution */

#ifdef __cplusplus
extern "C" {
#endif

typedef  int SigmaFunc(const char*,void*); /*GB*/

/*
 * C-interface functions
 */
extern char*  k_getar(void);
extern void   k_setar( size_t, char** );
extern char*  k_userid(void);

extern void   ku_alfa(void);
extern char*  ku_appl( int* luno, int* inmacro );

extern int    ku_bool( const char* expr );

extern int    ku_close( int luno );
extern void   ku_cmdl( const char* templ );

extern int    ku_edit( const char* path, int use_server );
extern char*  ku_eval( const char* expr );
extern int    ku_exec( const char* cmd );
extern int    ku_exel( const char* cmd );
extern char*  ku_expr( const char* expr );

extern char*  ku_getc(void);
extern char*  ku_gete(void);
extern char*  ku_getf(void);
extern int    ku_geti(void);
extern char*  ku_getl(void);
extern double ku_getr(void);
extern char*  ku_gets(void);

extern char*  ku_fcase( char* path );

extern char*  ku_home( const char* fname, const char* ftype );

extern char*  ku_inps( const char* prompt );
extern int    ku_inqf( const char* path );
extern int    ku_intr( int enable );

extern void   ku_last( const char* cmd );

extern int    ku_match( const char* string, const char* pattern,
                       int ignore_case );
extern int    ku_math( const char* expr, double* result );
extern int    ku_more( const char* question, const char* line );

extern int    ku_npar(void);

extern int    ku_open( int luno, const char* path, const char* mode );

extern void   ku_pad( const char* path, int del );
extern char*  ku_path(void);
extern char*  ku_name(void); /*GB*/
extern char*  ku_macro_path(void); /*GB*/
extern char** ku_getopts(const char* p,int* n); /*GB*/
extern void   ku_piaf( int socket, void(*sync)() );
extern char*  ku_proc( const char* prompt, const char* dfault );
extern char*  ku_prof( const char* prompt, const char* dfault );
extern int    ku_proi( const char* prompt, int dfault );
extern char*  ku_prop( const char* prompt );
extern double ku_pror( const char* prompt, double dfault );
extern char*  ku_pros( const char* prompt, const char* dfault );

extern char** ku_qenv(void);
extern char*  ku_qexe( const char* fname );
extern int    ku_qkey(void);
extern int    ku_qmac( const char* mname );

extern int    ku_read( int luno, char* buf, size_t len );

extern int    ku_sapp( const char* path, const char* exit );
extern void   ku_shut(void);
extern void   ku_sibr(void);
extern void   ku_spy( const char* option );
extern int    ku_stop( int set );

extern void   ku_time( time_t, clock_t );
extern void   ku_trap( int enable, int traceback );

extern int    ku_vqaddr( const char* vname );
extern int    ku_vtype( const char* vname );
extern int    ku_vvalue( const char* vname, double* value );

extern void   ku_whag(void);

extern void   ku_what( void(*styleG)() );
extern void   ku_write( int, const char* line );

extern int    km_icon( const char*, const char* );

extern void   ku_action_data( void* ); /*GB*/
extern void   ku_prompt( char*(*)(char*,void*),void*); /*GB*/
extern int    ku_exec_no_help( const char* cmd ); /*GB*/
extern void   ku_flush( int(*)(void*),void*); /*GB*/
extern void   ku_exit( int(*)(void*),void*); /*GB*/
extern void   ku_quit( int(*)(void*),void*); /*GB*/

extern void   ku_xvcreate( int(*)(void*),void* ); /*GB*/
extern void   ku_xvdelete( int(*)(void*),void* ); /*GB*/
extern void   ku_xvfind( int(*)(const char*,void*),void*); /*GB*/
extern void   ku_xvvalue( int(*)(const char*,double*,void*),void*); /*GB*/
extern void   ku_xvlist( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvcopy( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvinput( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvprint( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvread( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvwrite( int(*)(void*) ,void*); /*GB*/
extern void   ku_xvoperation( int(*)(void*) ,void*); /*GB*/

extern void   ku_xgl_setwidth(void(*)());              /*GB*/
extern void   ku_xgl_config(void(*)(const char*,int)); /*GB*/
extern void   ku_xgl_getline(char*(*)(const char*));   /*GB*/
extern void   ku_xgl_histadd(void(*)(const char*));    /*GB*/

extern void   ku_sigm(SigmaFunc,void*); /*GB*/
extern char*  ku_help(int*); /*GB*/
extern char*  ku_uhlp_file(); /*GB*/

extern int    ku_system( const char* cmd ); /*GB*/
  
#ifdef __cplusplus
}
#endif

#endif //kuip_kuser_h
