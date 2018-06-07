/*
 * $Id: kproto.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: kproto.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.3  2010/03/24 10:35:57  barrand
 * *** empty log message ***
 *
 * Revision 1.2  2009/11/18 13:59:33  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25 13:17:44  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */

extern char*  clean_word( char* );
extern char*  format_prompt( const char* );
/*G.Barrand : in the below kgl_ else stdio.h clash. */
/*
extern char*  kgl_getline( const char* );
extern void   kgl_config( const char*, int );
extern void   kgl_histadd( const char* );
extern void   kgl_setwidth( int );
*/
extern char*  input_line( const char*, char );
extern void   leave_kuip(void);
extern int    len_alias( const char*, int );
extern int    len_sysfun( const char* );
extern int    len_vector( const char* );
extern char*  quote_string( char*, int );
extern void   reset_break(void);
extern char*  split_at_semicolon( char* );

extern void   k_parse_args( const char*, char**, int* );


#ifndef vms
extern void   signal_handler( int );
#else
extern int    signal_handler( const void*, const void* );
extern int    control_C_ast( int );
#endif


