/*
 * $Id: getline.c,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: getline.c,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.11  2010/03/24 10:35:52  barrand
 * *** empty log message ***
 *
 * Revision 1.10  2009/11/18 14:02:27  barrand
 * *** empty log message ***
 *
 * Revision 1.9  2009/11/18 13:59:25  barrand
 * *** empty log message ***
 *
 * Revision 1.8  2008/11/05 19:51:51  barrand
 * *** empty log message ***
 *
 * Revision 1.7  2007/05/24 12:04:23  barrand
 * *** empty log message ***
 *
 * Revision 1.6  2007/05/24 12:01:26  barrand
 * *** empty log message ***
 *
 * Revision 1.5  2007/05/24 11:59:04  barrand
 * *** empty log message ***
 *
 * Revision 1.4  2007/05/24 11:54:43  barrand
 * *** empty log message ***
 *
 * Revision 1.3  2007/05/24 11:49:54  barrand
 * *** empty log message ***
 *
 * Revision 1.2  1999/11/08 11:21:52  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25  13:09:14  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:32:55  mclareni
 * Kuip
 *
 */
#if !defined(CERNLIB_IBMALL)
/*CMZ :  2.06/03 12/01/95  17.04.22  by  Gunter Folger*/
/*-- Author :*/

#include "kuip/kuip.h"
#include "kuip/kfor.h"
#include "kuip/kmenu.h"
#include "kuip/kflag.h"


#if 0
static char     rcsid[] =
"$Id: getline.c,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $";
static char    *copyright = "Copyright (C) 1991, 1992, Chris Thewalt";
#endif

/*
 * Copyright (C) 1991, 1992 by Chris Thewalt (thewalt@ce.berkeley.edu)
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted, provided
 * that the above copyright notices appear in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "as is" without express or
 * implied warranty.
 */

static int      kgl_tab();  /* forward reference needed for kgl_tab_hook */

/********************* exported interface ********************************/

/*G.Barrand : in the below kgl_ else stdio.h clash. */
extern char *kgl_getline( const char* );    /* read a line of input */
extern void  kgl_setwidth( int );        /* specify width of screen */
extern void  kgl_histadd( const char* ); /* adds entries to hist */

extern void  kgl_char_init();            /* get ready for no echo input */
extern void  kgl_char_cleanup();         /* undo kgl_char_init */
extern int   kgl_reset();                /* cleanup interrupted getline */
extern void  kgl_reinit();               /* reinitialize interrupted getline */

int             (*kgl_in_hook)() = 0;
int             (*kgl_out_hook)() = 0;
int             (*kgl_tab_hook)() = kgl_tab;

/******************** imported interface *********************************/

/*
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

extern int      isatty();
extern void    *malloc();
extern void     free();
extern int      raise();
*/

/******************** internal interface *********************************/

#define BUF_SIZE 1024

static int      kgl_init_done = -1;      /* terminal mode flag  */
static int      kgl_char_init_done = 0;  /* has kgl_char_init been called */
static int      kgl_termw = 80;          /* actual terminal width */
static int      kgl_scroll = 27;         /* width of EOL scrolling region */
static int      kgl_width = 0;           /* net size available for input */
static int      kgl_extent = 0;          /* how far to redraw, 0 means all */
static int      kgl_overwrite = 0;       /* overwrite mode */
static int      kgl_def_overwrite = 0;   /* default overwrite mode */
static int      kgl_edit_style = 0;      /* 0=ksh 1=DCL */
static int      kgl_no_echo = 0;         /* do not echo input characters */
static int      kgl_passwd = 0;          /* do not echo input characters */
static int      kgl_erase_line = 0;      /* erase line before returning */
static int      kgl_pos, kgl_cnt = 0;     /* position and size of input */
static char     kgl_buf[BUF_SIZE];       /* input buffer */
static char     kgl_killbuf[BUF_SIZE]=""; /* killed text */
static char     kgl_outbuf[BUF_SIZE]=""; /* output buffer for kgl_putc() */
static char    *kgl_outptr = kgl_outbuf;  /* ptr to next free pos. in outbuf */
static const char *kgl_prompt;           /* to save the prompt string */
static int      kgl_intrc = 0;           /* keyboard SIGINT char */
static int      kgl_quitc = 0;           /* keyboard SIGQUIT char */
static int      kgl_suspc = 0;           /* keyboard SIGTSTP char */
static int      kgl_dsuspc = 0;          /* delayed SIGTSTP char */
static int      kgl_search_mode = 0;     /* search mode flag */


static void     kgl_init();              /* prepare to edit a line */
static void     kgl_cleanup();           /* to undo kgl_init */

static void     kgl_addchar();           /* install specified char */
static void     kgl_del();               /* del, either left (-1) or cur (0) */
static void     kgl_delword();           /* delete word */
static void     kgl_error();             /* write error msg and die */
static void     kgl_fixup( const char*, int, int);
                                        /* fixup state variables and screen */
static int      kgl_getc();              /* read one char from terminal */
static void     kgl_kill();              /* delete to EOL */
static void     kgl_newline();           /* handle \n or \r */
static void     kgl_putc();              /* write one char to terminal */
static void     kgl_puts();              /* write a line to terminal */
static void     kgl_flush();             /* write out kgl_putc() buffer */
static void     kgl_redraw();            /* issue \n and redraw all */
static void     kgl_transpose();         /* transpose two chars */
static void     kgl_yank();              /* yank killed text */

static void     hist_init();    /* initializes hist pointers */
static char    *hist_next();    /* return ptr to next item */
static char    *hist_prev();    /* return ptr to prev item */
static char    *hist_save();    /* makes copy of a string, without NL */

static void     search_addchar();       /* increment search string */
static void     search_term();          /* reset with current contents */
static void     search_back();          /* look back for current string */
static void     search_forw();          /* look forw for current string */

/************************ nonportable part *********************************/

/*
extern int      write();
extern void     exit();
*/

#ifdef TERMIO_POSIX
#include <termios.h>
static struct termios  new_termios, old_termios;
#endif

#ifdef TERMIO_BSD
#include <sys/ioctl.h>
#include <sgtty.h>
static struct sgttyb   new_tty, old_tty;
static struct tchars   tch;
static struct ltchars  ltch;
static int  typeahead_count;
static char typeahead_buffer[1024];
#endif

#ifdef TERMIO_SYSV
#include <sys/ioctl.h>
#include <termio.h>
static struct termio   new_termio, old_termio;
#endif

#ifdef vms
static int   setbuff[2];             /* buffer to set terminal attributes */
static short chan = -1;              /* channel to terminal */
static int   enable_ast;             /* reenable ^C AST */
static struct dsc$descriptor_s descrip;     /* VMS descriptor */
#endif

void
  kgl_config( const char *which, int value )
{
  if( strcmp( which, "overwrite" ) == 0 )
    kgl_def_overwrite = value;
  else if( strcmp( which, "style" ) == 0 )
    kgl_edit_style = value;
  else if( strcmp( which, "noecho" ) == 0 )
    kgl_no_echo = value;
  else if( strcmp( which, "erase" ) == 0 )
    kgl_erase_line = value;
  else
    kprintf( "kgl_config: %s ?\n", which );
}


void
  kgl_char_init()          /* turn off input echo */
{
   if (isatty(0) == 0 || isatty(1) == 0)
      return;

#ifdef vms
    kgl_intrc = 'C' - '@';       /* ^C */
    kgl_quitc = 'Y' - '@' + 128; /* ^Y quit in DCL and yank in KSH */
#endif

#ifdef MSDOS
    kgl_intrc = 'C' - '@';       /* ^C */
    kgl_quitc = 'Y' - '@';        /* ^Y quit in DCL and yank in KSH */
#endif

#ifdef TERMIO_POSIX
    tcgetattr(0, &old_termios);
    kgl_intrc = old_termios.c_cc[VINTR];
    kgl_quitc = old_termios.c_cc[VQUIT];
# ifdef VSUSP
    kgl_suspc = old_termios.c_cc[VSUSP];
# endif
# ifdef VDSUSP
    kgl_dsuspc = old_termios.c_cc[VDSUSP];
# endif
    new_termios = old_termios;
    new_termios.c_iflag &= ~(BRKINT|ISTRIP|IXON|IXOFF|INLCR|IGNCR|ICRNL);
    new_termios.c_iflag |= (IGNBRK|IGNPAR);
    new_termios.c_lflag &= ~(ICANON|ISIG|IEXTEN|ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
#endif

#ifdef TERMIO_BSD
    ioctl(0, TIOCGETC, &tch);
    ioctl(0, TIOCGLTC, &ltch);
    kgl_intrc = tch.t_intrc;
    kgl_quitc = tch.t_quitc;
    kgl_suspc = ltch.t_suspc;
    kgl_dsuspc = ltch.t_dsuspc;
    ioctl(0, TIOCGETP, &old_tty);
    new_tty = old_tty;
    new_tty.sg_flags |= RAW;
    new_tty.sg_flags &= ~ECHO;
#endif

#ifdef TERMIO_SYSV
    ioctl(0, TCGETA, &old_termio);
    kgl_intrc = old_termio.c_cc[VINTR];
    kgl_quitc = old_termio.c_cc[VQUIT];
    new_termio = old_termio;
    new_termio.c_iflag &= ~(BRKINT|ISTRIP|IXON|IXOFF);
    new_termio.c_iflag |= (IGNBRK|IGNPAR);
    new_termio.c_lflag &= ~(ICANON|ISIG|ECHO);
    new_termio.c_cc[VMIN] = 1;
    new_termio.c_cc[VTIME] = 0;
#endif

    kgl_char_init_done = 1;
}

static void
  kgl_char_reinit()      /* turn off input echo */
{
#ifdef TERMIO_POSIX
    tcsetattr(0, TCSANOW, &new_termios);
#endif

#ifdef TERMIO_BSD
    /*
     * switching the BSD driver to RAW mode discards the input queue
     * we save the type-ahead in a buffer which kgl_getc() uses
     */
    typeahead_count = 0;

    while( 1 ) {
      fd_set readfds;
      struct timeval timeout;

      FD_ZERO( &readfds );
      FD_SET( 0, &readfds );
      timeout.tv_sec = 0;
      timeout.tv_usec = 0;

      if( select( 1, SELECT_CAST(&readfds), NULL, NULL, &timeout ) > 0 ) {
        read( 0, &typeahead_buffer[typeahead_count++], 1 );
      }
      else
        break;
    }

    ioctl(0, TIOCSETP, &new_tty);
#endif

#ifdef TERMIO_SYSV
    ioctl(0, TCSETA, &new_termio);
#endif

#ifdef vms
    /* cancel pending ^C AST */
    enable_ast = control_C_ast( 0 );
    var_descriptor( descrip, "TT:" );
    (void)sys$assign(&descrip,&chan,0,0);
    (void)sys$qiow(0,chan,IO$_SENSEMODE,0,0,0,setbuff,8,0,0,0,0);
    setbuff[1] |= TT$M_NOECHO;
    (void)sys$qiow(0,chan,IO$_SETMODE,0,0,0,setbuff,8,0,0,0,0);

    /*
     * Install exit() as ^Y AST which is called when the input terminal
     * hangs up, e.g. by closing the window before leaving the application.
     */
    sys$qiow( 0, chan, IO$_SETMODE | IO$M_CTRLYAST, 0, 0, 0,
             exit, 0, PSL$C_USER, 0, 0, 0 );
#endif /* vms */
}

void
  kgl_char_cleanup()     /* undo effects of kgl_char_init */
{
   if (isatty(0) == 0 || isatty(1) == 0)
      return;

#ifdef TERMIO_POSIX
    tcsetattr(0, TCSANOW, &old_termios);
#endif

#ifdef TERMIO_BSD
    ioctl(0, TIOCSETP, &old_tty);
#endif

#ifdef TERMIO_SYSV
    ioctl(0, TCSETA, &old_termio);
#endif

#ifdef vms
    setbuff[1] &= ~TT$M_NOECHO;
    (void)sys$qiow(0,chan,IO$_SETMODE,0,0,0,setbuff,8,0,0,0,0);
    sys$dassgn(chan);
    chan = -1;
    /* reenable ^C AST */
    control_C_ast( enable_ast );
#endif
}

static void
kgl_flush()
/* flush kgl_putc() buffer */
{
    if( kgl_outptr != kgl_outbuf ) {
      if(write( 1, kgl_outbuf, kgl_outptr - kgl_outbuf )){} /*G.Barrand*/
      kgl_outptr = kgl_outbuf;
    }
}


/* execute the idle command */
static
int kgl_idle()
{
  int old_timeout = kc_value.idle_time;
  kc_value.idle_time = 0;           /* in case idle command causes a prompt */

  kgl_fixup( kgl_prompt, -1, 0 );
  kgl_puts( kc_value.idle_cmd );
  kgl_putc( '\n' );
  kgl_cleanup();
  ku_exel( kc_value.idle_cmd );
  kgl_init();
  kgl_fixup( kgl_prompt, -2, kgl_pos );

  kc_value.idle_time = old_timeout;
  return 0;
}


static int
kgl_getc()
/* get a character without echoing it to screen */
{
#ifdef MSDOS
# define k_ctrl_C   3
# define k_ctrl_Z  26
# define k_ctrl_Q  17
# define k_ctrl_K  11
# define k_rt_arr -77
# define k_lt_arr -75
# define k_up_arr -72
# define k_dn_arr -80
# define k_PGUP   -73
# define k_PGDW   -81
# define k_HOME   -71
# define k_END    -79
# define k_INS    -82
# define k_DEL    -83
# define k_ENTER   13
# define k_CR      13
# define k_BS       8
# define k_ESC     27
# define k_alt_H  -35
# define k_beep     7
# ifndef WINNT
    int get_cursor__(int *,int *);
    int display_off__(int *);
    int display_on__();
    int locate_(int *,int *);
    int ixc, iyc;
# endif
    int pause_();
#endif


#ifdef vms
    short iosb[4];
#endif

    int             c;
    char            ch;

    kgl_flush();

#ifdef UNIX

# ifdef TERMIO_BSD
    if( typeahead_count > 0 ) {
      int i;
      c = typeahead_buffer[0];
      typeahead_count--;
      for( i = 0; i < typeahead_count; i++ )
        typeahead_buffer[i] = typeahead_buffer[i+1];
      return c;
    }
# endif

# ifndef MSDOS
    do {

#  ifdef HAVE_SELECT
      if( kc_value.idle_time > 0 ) {
        fd_set readfds;
        struct timeval timeout;

        FD_ZERO( &readfds );
        FD_SET( 0, &readfds );
        timeout.tv_sec = kc_value.idle_time;
        timeout.tv_usec = 0;

        if( select( 1, SELECT_CAST(&readfds), NULL, NULL, &timeout ) == 0 )
          return kgl_idle();
      }
#  endif

       errno = 0;
       c = (read(0, &ch, 1) > 0)? ch : -1;
    } while (errno == EINTR);

# else /* MSDOS */
    c = pause_();
    if (c < 0) {
        switch (c) {
          case k_up_arr: c =  16;   /* up -> ^P */
            break;
          case k_dn_arr: c =  14;   /* down -> ^N */
            break;
          case k_lt_arr: c =   2;   /* left -> ^B */
            break;
          case k_rt_arr: c =   6;   /* right -> ^F */
            break;
          case k_INS:    c =  15;   /* right -> ^F */
            break;
          case k_DEL:    c =   4;   /* Delete character under cursor */
            break;
          case k_END:    c =   5;   /* Moves cursor to end of line */
            break;
          case k_HOME:   c =   1;   /* Moves cursor to beginning of line */
            break;
#  ifndef WINNT
          case k_PGUP: c = 0; display_on__();
            break;
          case k_PGDW: c =0; get_cursor__(&ixc, &iyc);
                             display_off__(&iyc);
                             locate_(&ixc,&iyc);
            break;
#  endif
          default: c = 0;    /* make it garbage */
        }
    }
    else {
      switch(c) {
          case k_ESC:    c =  'U' - '@'; /* Clear full line  -> ^U */
            break;
          case k_ctrl_C:   raise(SIGINT);          /* Ctrl-C handle */
            break;
          default:
            break;
        }
    }
# endif
#endif

#ifdef vms
    c = '\0';
    if( kc_value.idle_time > 0 ) {
      sys$qiow( 0, chan, IO$_TTYREADALL | IO$M_TIMED, iosb, 0, 0,
               &c, 1, kc_value.idle_time + 1, 0, 0, 0 );
      if( iosb[0] == SS$_TIMEOUT )
        return kgl_idle();
    }
    else {
      sys$qiow( 0, chan, IO$_TTYREADALL, iosb, 0, 0,
               &c, 1, 0, 0, 0, 0 );
    }
    if( iosb[0] == SS$_NORMAL )
      c &= 0177;                /* get a char */
    else
      c = -1;                   /* EOF */
#endif

    return c;
}

static void
  kgl_putc(c)
int c;
{
    char   ch = c;

    if( kgl_passwd && isgraph( c ) )
      ch = ' ';

    /* buffer characters because VMS C-I/O is sooo slow */
    *kgl_outptr++ = ch;

#ifdef TERMIO_MAP_NL
    if (ch == '\n') {           /* BSD in RAW mode, map NL to NL,CR */
        *kgl_outptr++ = '\r';
    }
#endif

    *kgl_outptr = '\0';
}

/******************** fairly portable part *********************************/

static void
  kgl_puts(buf)
char *buf;
{
/*
    int len = strlen(buf);

    write(1, buf, len);
*/
    while( *buf != '\0' )
      kgl_putc( *buf++ );
}

static void
  kgl_error(buf)
char *buf;
{
    int len = strlen(buf);

    kgl_cleanup();
    if(write(2, buf, len)) {} /*G.Barrand*/
    /* exit(1); */
}

static void
  kgl_init()       /* set up variables and terminal */
{
    if (kgl_init_done < 0) {             /* -1 only on startup */
        hist_init();
    }

    if (!kgl_char_init_done)
       kgl_char_init();

    kgl_char_reinit();
    kgl_init_done = 1;
    kgl_overwrite = kgl_def_overwrite;
    kgl_outptr = kgl_outbuf;      /* reset kgl_putc() buffer */
}

static void
  kgl_cleanup()       /* undo effects of kgl_init, as necessary */
{
    kgl_flush();
    if (kgl_init_done > 0)
        kgl_char_cleanup();
    kgl_init_done = 0;
}

void
  kgl_reinit()        /* reinitialize terminal */
{

    if (isatty(0) == 0 || isatty(1) == 0)
       return;

    if (kgl_init_done == 0) {
       kgl_char_reinit();
       kgl_init_done = 1;
    }
}

int
kgl_reset()                    /* undo effects of kgl_init, as necessary */
{
    int set = 0;

    if (isatty(0) == 0 || isatty(1) == 0)
       return set;

    if (kgl_init_done > 0) {
        kgl_char_cleanup();
        kgl_init_done = 0;
        set = 1;
    }

    return set;
}

void
  kgl_setwidth(w)
int w;
{
    if (w > 20) {
        kgl_termw = w;
        kgl_scroll = w / 3;
    } else {
        kgl_error("\n*** Error: minimum screen width is 21\n");
    }
}

char *kgl_getline( const char *prompt )
{
    int             c, loc, tmp;
    int             sig;

    if( prompt != NULL )
      kgl_prompt = prompt;
    else
      kgl_prompt = "";

    kgl_buf[0] = 0;
    kgl_init();
    if (kgl_in_hook)
        kgl_in_hook(kgl_buf);
    kgl_fixup(kgl_prompt, -2, BUF_SIZE);
    while ((c = kgl_getc()) != -1) {  /* -1 for EOF */
        kgl_extent = 0;          /* reset to full extent */
        if (isprint(c)) {
            if (kgl_search_mode)
               search_addchar(c);
            else
               kgl_addchar(c);
        } else {
            int dcl_xlate = (kgl_edit_style == 1);

            if (kgl_search_mode) {
                if (c == '\033' || c == '\016' || c == '\020') {
                    search_term();
                    c = 0;              /* ignore the character */
                } else if (c == '\010' || c == '\177') {
                    search_addchar(-1); /* unwind search string */
                    c = 0;
                } else if (c != '\022' && c != '\023') {
                    search_term();      /* terminate and handle char */
                }
            }

            if( c == '\033' ) {
              c = kgl_getc();
              if (c == '[' || c == 'O')
                c = kgl_getc();

              switch( c ) {
              case 'A':       /* up */
                c = 'P' - '@';
                dcl_xlate = 0;
                break;
              case 'B':       /* down */
                c = 'N' - '@';
                dcl_xlate = 0;
                break;
              case 'C':       /* right */
                c = 'F' - '@';
                dcl_xlate = 0;
                break;
              case 'D':       /* left */
                c = 'B' - '@';
                dcl_xlate = 0;
                break;
              default:        /* who knows */
                kgl_putc('\007');
                c = 0;
                break;

              case '2':
                c = kgl_getc();
                if( kgl_getc() == '~' ) { /* DEC function keys */
                  switch( c ) {
                  case '4':       /* F12 == ^H */
                    c = 'H' - '@';
                    break;
                  case '5':       /* F13 == ^J */
                    c = 'J' - '@';
                    break;
                  case '6':       /* F14 == ^A */
                    c = 'A' - '@';
                    break;
                  default:
                    kgl_putc('\007');
                    c = 0;
                    break;
                  }
                }
              }
            }

            if( c != 0 && dcl_xlate ) {
              /* translate DCL control characters */

              switch( c ) {
              case '\001':      /* ^A toggles overwrite mode */
                c = 'O' - '@';
                break;
              case '\002':      /* ^B recall previous line */
                c = 'P' - '@';
                break;
              case '\004':      /* ^D moves cursor left */
                c = 'B' - '@';
                break;
              case '\010':      /* ^H moves cursor to beginning of line */
                c = 'A' - '@';
                break;
              case '\012':      /* ^J deletes previous word */
                kgl_delword();
                c = 0;
                break;
              case '\022':      /* ^R refreshes line */
                c = 'L' - '@';
                break;
              case '\030':      /* ^X flush type-ahead then ^U */
              case '\025':      /* ^U deletes from beginning of line */
                strcpy( &kgl_buf[0], &kgl_buf[kgl_pos] );
                kgl_fixup( kgl_prompt, 0, 0 );
                c = 0;
                break;
              case '\031':      /* ^Y aborts */
                c = kgl_quitc;
                break;
              case '\032':      /* ^Z is EOF */
                c = 'D' - '@';
                break;
                /* the following codes are the same in both modes */
              case '\003':      /* ^C interrupts */
              case '\005':      /* ^E moves cursor to end of line */
              case '\006':      /* ^F moves cursor right */
              case '\011':      /* ^I moves to next tab position */
              case '\015':      /* ^M terminates input */
              case '\016':      /* ^N recalls next command */
              case '\177':      /* DEL deletes character */
                break;
              default:          /* the rest rings a bell */
                c = 'G' - '@';
              }
            }

            switch (c) {
              case '\n': case '\r':                     /* newline */
                kgl_newline();
                kgl_cleanup();
                return kgl_buf;
                /*NOTREACHED*/
                break;
              case '\001': kgl_fixup(kgl_prompt, -1, 0);          /* ^A */
                break;
              case '\002': kgl_fixup(kgl_prompt, -1, kgl_pos-1);   /* ^B */
                break;
              case '\004':                                      /* ^D */
                if (kgl_cnt == 0) {
                  /* ignore ^D for completely empty line
                    kgl_buf[0] = 0;
                    kgl_cleanup();
                    kgl_putc('\n');
                    return kgl_buf; */
                } else {
                    kgl_del(0);
                }
                break;
              case '\005': kgl_fixup(kgl_prompt, -1, kgl_cnt);     /* ^E */
                break;
              case '\006': kgl_fixup(kgl_prompt, -1, kgl_pos+1);   /* ^F */
                break;
              case '\010': case '\177': kgl_del(-1);     /* ^H and DEL */
                break;
              case '\t':                                        /* TAB */
                if (kgl_tab_hook) {
                    tmp = kgl_pos;
                    loc = kgl_tab_hook(kgl_buf, strlen(kgl_prompt), &tmp);
                    if (loc >= 0 || tmp != kgl_pos)
                        kgl_fixup(kgl_prompt, loc, tmp);
                }
                break;
              case '\013': kgl_kill();                           /* ^K */
                break;
              case '\014': kgl_redraw();                         /* ^L */
                break;
              case '\016':                                      /* ^N */
                strcpy(kgl_buf, hist_next());
                if (kgl_in_hook)
                    kgl_in_hook(kgl_buf);
                kgl_fixup(kgl_prompt, 0, BUF_SIZE);
                break;
              case '\017': kgl_overwrite = !kgl_overwrite;        /* ^O */
                break;
              case '\020':                                      /* ^P */
                strcpy(kgl_buf, hist_prev());
                if (kgl_in_hook)
                    kgl_in_hook(kgl_buf);
                kgl_fixup(kgl_prompt, 0, BUF_SIZE);
                break;
              case '\022': search_back(1);                      /* ^R */
                break;
              case '\023': search_forw(1);                      /* ^S */
                break;
              case '\024': kgl_transpose();                      /* ^T */
                break;
              case '\025': kgl_fixup(kgl_prompt,-1,0); kgl_kill(); /* ^U */
                break;
              case '\031': kgl_yank();                           /* ^Y */
                break;
              default:          /* check for a terminal signal */
#ifndef MSDOS
                if (c > 0) {    /* ignore 0 (reset above) */
                    sig = 0;
# ifndef vms
#  ifdef SIGINT
                    if (c == kgl_intrc)
                        sig = SIGINT;
#  endif
#  ifdef SIGQUIT
                    if (c == kgl_quitc)
                        sig = SIGQUIT;
#  endif
#  ifdef SIGTSTP
                    if (c == kgl_suspc || c == kgl_dsuspc)
                        sig = SIGTSTP;
#  endif
# else  /* vms */
                    if (c == kgl_intrc)
                        sig = SS$_DEBUG;
                    if (c == kgl_quitc)
                        sig = SS$_CONTROLY;
# endif /* vms */

                    if (sig != 0) {
                        kgl_cleanup();
# ifndef vms
#  ifdef SIGTSTP
                        /*
                         * Send signal to process group to propagate ^Z to the
                         * interactive shell in case the program was started
                         * from a shell script.
                         */
#   ifdef GETPGRP_BSD
                        kill( -getpgrp( 0 ), sig );
#   else
                        kill( -getpgrp(), sig );
#   endif
#  else
                        raise(sig);
#  endif
# else
                        lib$signal( sig );
# endif
                        kgl_init();
                        kgl_redraw();
                        c = 0;
                    }
                }
#endif
                if (c != 0)   /*  warn user for strange character typed */
                    kgl_putc('\007');
                break;
            }
        }
    }
    kgl_cleanup();
    return NULL;                /* EOF reached */
}

static void
  kgl_addchar(c)
int c;
/* adds the character c to the input buffer at current location */
{
    int  i;

    if (kgl_cnt >= BUF_SIZE - 1)
        kgl_error("\n*** Error: getline(): input buffer overflow\n");
    if (kgl_overwrite == 0 || kgl_pos == kgl_cnt) {
        for (i=kgl_cnt; i >= kgl_pos; i--)
            kgl_buf[i+1] = kgl_buf[i];
        kgl_buf[kgl_pos] = c;
        kgl_fixup(kgl_prompt, kgl_pos, kgl_pos+1);
    } else {
        kgl_buf[kgl_pos] = c;
        kgl_extent = 1;
        kgl_fixup(kgl_prompt, kgl_pos, kgl_pos+1);
    }
}

static void
  kgl_yank()
/* adds the kill buffer to the input buffer at current location */
{
    int  i, len;

    len = strlen(kgl_killbuf);
    if (len > 0) {
        if (kgl_overwrite == 0) {
            if (kgl_cnt + len >= BUF_SIZE - 1)
                kgl_error("\n*** Error: getline(): input buffer overflow\n");
            for (i=kgl_cnt; i >= kgl_pos; i--)
                kgl_buf[i+len] = kgl_buf[i];
            for (i=0; i < len; i++)
                kgl_buf[kgl_pos+i] = kgl_killbuf[i];
            kgl_fixup(kgl_prompt, kgl_pos, kgl_pos+len);
        } else {
            if (kgl_pos + len > kgl_cnt) {
                if (kgl_pos + len >= BUF_SIZE - 1)
                    kgl_error("\n*** Error: getline(): input buffer overflow\n");
                kgl_buf[kgl_pos + len] = 0;
            }
            for (i=0; i < len; i++)
                kgl_buf[kgl_pos+i] = kgl_killbuf[i];
            kgl_extent = len;
            kgl_fixup(kgl_prompt, kgl_pos, kgl_pos+len);
        }
    } /* else
        kgl_putc('\007'); */
}

static void
  kgl_transpose()
/* switch character under cursor and to left of cursor */
{
    int    c;

    if (kgl_pos > 0 && kgl_cnt > kgl_pos) {
        c = kgl_buf[kgl_pos-1];
        kgl_buf[kgl_pos-1] = kgl_buf[kgl_pos];
        kgl_buf[kgl_pos] = c;
        kgl_extent = 2;
        kgl_fixup(kgl_prompt, kgl_pos-1, kgl_pos);
    } /* else
        kgl_putc('\007'); */
}

static void
  kgl_newline()
/*
 * Cleans up entire line before returning to caller. A \n is appended.
 * If line longer than screen, we redraw starting at beginning
 */
{
    int change = kgl_cnt;
    int len = kgl_cnt;
    int loc = kgl_width - 5;     /* shifts line back to start position */

    if (kgl_cnt >= BUF_SIZE - 1)
        kgl_error("\n*** Error: getline(): input buffer overflow\n");
    if (kgl_out_hook) {
        change = kgl_out_hook(kgl_buf);
        len = strlen(kgl_buf);
    }
    if( kgl_erase_line ) {
      char kgl_buf0 = kgl_buf[0];
      kgl_buf[0] = '\0';
      kgl_fixup("", 0, 0);
      kgl_buf[0] = kgl_buf0;
    }
    else {
      if (loc > len)
        loc = len;
      kgl_fixup(kgl_prompt, change, loc); /* must do this before appending \n */
      kgl_putc('\n');
    }
    kgl_buf[len] = '\n';
    kgl_buf[len+1] = '\0';
}

static void
  kgl_del(loc)
int loc;
/*
 * Delete a character.  The loc variable can be:
 *    -1 : delete character to left of cursor
 *     0 : delete character under cursor
 */
{
    int i;

    if ((loc == -1 && kgl_pos > 0) || (loc == 0 && kgl_pos < kgl_cnt)) {
        for (i=kgl_pos+loc; i < kgl_cnt; i++)
            kgl_buf[i] = kgl_buf[i+1];
        kgl_fixup(kgl_prompt, kgl_pos+loc, kgl_pos+loc);
    } /* else
        kgl_putc('\007'); */
}

static void
  kgl_delword()
/*
 * Delete previous word (^J/LF on VMS)
 */
{
    static char punct[] = " `!$&(-=+]}\\|;:'\",<.>?";
    int i = kgl_pos;

    while( i > 0 && strchr( punct, kgl_buf[i-1] ) != NULL )
      i--;
    while( i > 0 && strchr( punct, kgl_buf[i-1] ) == NULL )
      i--;

    strcpy( kgl_buf + i, kgl_buf + kgl_pos );
    kgl_fixup(kgl_prompt, i, i );
}

static void
  kgl_kill()
/* delete from current position to the end of line */
{
    if (kgl_pos < kgl_cnt) {
        strcpy(kgl_killbuf, kgl_buf + kgl_pos);
        kgl_buf[kgl_pos] = '\0';
        kgl_fixup(kgl_prompt, kgl_pos, kgl_pos);
    } /* else
        kgl_putc('\007'); */
}

static void
  kgl_redraw()
/* emit a newline, reset and redraw prompt and current input line */
{
    if (kgl_init_done > 0) {
        kgl_putc('\n');
        kgl_fixup(kgl_prompt, -2, kgl_pos);
    }
}

static void
  kgl_fixup( const char *prompt,
           int change,
           int cursor )
/*
 * This function is used both for redrawing when input changes or for
 * moving within the input line.  The parameters are:
 *   prompt:  compared to last_prompt[] for changes;
 *   change : the index of the start of changes in the input buffer,
 *            with -1 indicating no changes, -2 indicating we're on
 *            a new line, redraw everything.
 *   cursor : the desired location of the cursor after the call.
 *            A value of BUF_SIZE can be used  to indicate the cursor should
 *            move just past the end of the input line.
 */
{
    static int   kgl_shift;      /* index of first on screen character */
    static int   off_right;     /* true if more text right of screen */
    static int   off_left;      /* true if more text left of screen */
    static char *last_prompt = NULL;
    int          left = 0, right = -1;          /* bounds for redraw */
    int          pad;           /* how much to erase at end of line */
    int          backup;        /* how far to backup before fixing */
    int          new_shift;     /* value of shift based on cursor */
    int          extra;         /* adjusts when shift (scroll) happens */
    int          i;
    int          new_right = -1; /* alternate right bound, using kgl_extent */
    int          l1, l2;

    if( last_prompt == NULL )
      last_prompt = strdup( "" );

    if (change == -2) {   /* reset */
        kgl_pos = kgl_cnt = kgl_shift = off_right = off_left = 0;
        kgl_passwd = 0;
        kgl_puts((char*)prompt); /*G.Barrand : clang : cast*/
        kgl_passwd = kgl_no_echo;
        free( last_prompt );
        last_prompt = strdup( prompt );
        change = 0;
        kgl_width = kgl_termw - strlen(prompt);
    } else if (strcmp(prompt, last_prompt) != 0) {
        l1 = strlen(last_prompt);
        l2 = strlen(prompt);
        kgl_cnt = kgl_cnt + l1 - l2;
        free( last_prompt );
        last_prompt = strdup( prompt );
        backup = kgl_pos - kgl_shift + l1;
        for (i=0; i < backup; i++)
            kgl_putc('\b');
        kgl_passwd = 0;
        kgl_puts((char*)prompt); /*G.Barrand : clang : cast*/
        kgl_passwd = kgl_no_echo;
        kgl_pos = kgl_shift;
        kgl_width = kgl_termw - l2;
        change = 0;
    }
    pad = (off_right)? kgl_width - 1 : kgl_cnt - kgl_shift;   /* old length */
    backup = kgl_pos - kgl_shift;
    if (change >= 0) {
        kgl_cnt = strlen(kgl_buf);
        if (change > kgl_cnt)
            change = kgl_cnt;
    }
    if (cursor > kgl_cnt) {
        /* BUF_SIZE means end of line */
        /* if (cursor != BUF_SIZE)
            kgl_putc('\007'); */
        cursor = kgl_cnt;
    }
    if (cursor < 0) {
        /* kgl_putc('\007'); */
        cursor = 0;
    }
    if (off_right || (off_left && cursor < kgl_shift + kgl_width - kgl_scroll / 2))
        extra = 2;                      /* shift the scrolling boundary */
    else
        extra = 0;
    new_shift = cursor + extra + kgl_scroll - kgl_width;
    if (new_shift > 0) {
        new_shift /= kgl_scroll;
        new_shift *= kgl_scroll;
    } else
        new_shift = 0;
    if (new_shift != kgl_shift) {        /* scroll occurs */
        kgl_shift = new_shift;
        off_left = (kgl_shift)? 1 : 0;
        off_right = (kgl_cnt > kgl_shift + kgl_width - 1)? 1 : 0;
        left = kgl_shift;
        new_right = right = (off_right)? kgl_shift + kgl_width - 2 : kgl_cnt;
    } else if (change >= 0) {           /* no scroll, but text changed */
        if (change < kgl_shift + off_left) {
            left = kgl_shift;
        } else {
            left = change;
            backup = kgl_pos - change;
        }
        off_right = (kgl_cnt > kgl_shift + kgl_width - 1)? 1 : 0;
        right = (off_right)? kgl_shift + kgl_width - 2 : kgl_cnt;
        new_right = (kgl_extent && (right > left + kgl_extent))?
                     left + kgl_extent : right;
    }
    pad -= (off_right)? kgl_width - 1 : kgl_cnt - kgl_shift;
    pad = (pad < 0)? 0 : pad;
    if (left <= right) {                /* clean up screen */
        for (i=0; i < backup; i++)
            kgl_putc('\b');
        if (left == kgl_shift && off_left) {
            kgl_putc('$');
            left++;
        }
        for (i=left; i < new_right; i++)
            kgl_putc(kgl_buf[i]);
        kgl_pos = new_right;
        if (off_right && new_right == right) {
            kgl_putc('$');
            kgl_pos++;
        } else {
            for (i=0; i < pad; i++)     /* erase remains of prev line */
                kgl_putc(' ');
            kgl_pos += pad;
        }
    }
    i = kgl_pos - cursor;                /* move to final cursor location */
    if (i > 0) {
        while (i--)
           kgl_putc('\b');
    } else {
        for (i=kgl_pos; i < cursor; i++)
            kgl_putc(kgl_buf[i]);
    }
    kgl_pos = cursor;
}

static int
  kgl_tab(buf, offset, loc)
char  *buf;
int    offset;
int   *loc;
/* default tab handler, acts like tabstops every 8 cols */
{
    int i, count, len;

    len = strlen(buf);
    count = 8 - (offset + *loc) % 8;
    for (i=len; i >= *loc; i--)
        buf[i+count] = buf[i];
    for (i=0; i < count; i++)
        buf[*loc+i] = ' ';
    i = *loc;
    *loc = i + count;
    return i;
}

/******************* History stuff **************************************/

#ifndef HIST_SIZE
#define HIST_SIZE 100
#endif

static int      hist_pos = 0, hist_last = 0;
static char    *hist_buf[HIST_SIZE];

static void
  hist_init()
{
    int i;

    hist_buf[0] = "";
    for (i=1; i < HIST_SIZE; i++)
        hist_buf[i] = (char *)0;
}

void
  kgl_histadd( const char *buf )
{
    static char *prev = 0;
    const char *p = buf;
    int len;

    while (*p == ' ' || *p == '\t' || *p == '\n')
        p++;
    if (*p) {
        len = strlen(buf);
        if (strchr(p, '\n'))    /* previously line already has NL stripped */
            len--;
        if (prev == 0 || strlen(prev) != len ||
                            strncmp(prev, buf, len) != 0) {
            char *pbuf = (char*)buf; /*G.Barrand : to rm warning.*/
            hist_buf[hist_last] = hist_save(pbuf);
            prev = hist_buf[hist_last];
            hist_last = (hist_last + 1) % HIST_SIZE;
            if (hist_buf[hist_last] && *hist_buf[hist_last]) {
                free(hist_buf[hist_last]);
            }
            hist_buf[hist_last] = "";
        }
    }
    hist_pos = hist_last;
}

static char *
  hist_prev()
/* loads previous hist entry into input buffer, sticks on first */
{
    char *p = 0;
    int   next = (hist_pos - 1 + HIST_SIZE) % HIST_SIZE;

    if (hist_buf[hist_pos] != 0 && next != hist_last) {
        hist_pos = next;
        p = hist_buf[hist_pos];
    }
    if (p == 0) {
        p = "";
        /* kgl_putc('\007'); */
    }
    return p;
}

static char *
  hist_next()
/* loads next hist entry into input buffer, clears on last */
{
    char *p = 0;

    if (hist_pos != hist_last) {
        hist_pos = (hist_pos+1) % HIST_SIZE;
        p = hist_buf[hist_pos];
    }
    if (p == 0) {
        p = "";
        /* kgl_putc('\007'); */
    }
    return p;
}

static char *
  hist_save(p)
char *p;
/* makes a copy of the string */
{
    char *s;
    int   len = strlen(p);
    char *nl = strchr(p, '\n');

    if (nl) {
        if ((s = malloc(len)) != 0) {
            strncpy(s, p, len-1);
            s[len-1] = 0;
        }
    } else {
        if ((s = malloc(len+1)) != 0) {
            strcpy(s, p);
        }
    }
    if (s == 0)
        kgl_error("\n*** Error: hist_save() failed on malloc\n");
    return s;
}

/******************* Search stuff **************************************/

static char  search_prompt[101];  /* prompt includes search string */
static char  search_string[100];
static int   search_pos = 0;      /* current location in search_string */
static int   search_forw_flg = 0; /* search direction flag */
static int   search_last = 0;     /* last match found */

static void
  search_update(c)
int c;
{
    if (c == 0) {
        search_pos = 0;
        search_string[0] = 0;
        search_prompt[0] = '?';
        search_prompt[1] = ' ';
        search_prompt[2] = 0;
    } else if (c > 0) {
        search_string[search_pos] = c;
        search_string[search_pos+1] = 0;
        search_prompt[search_pos] = c;
        search_prompt[search_pos+1] = '?';
        search_prompt[search_pos+2] = ' ';
        search_prompt[search_pos+3] = 0;
        search_pos++;
    } else {
        if (search_pos > 0) {
            search_pos--;
            search_string[search_pos] = 0;
            search_prompt[search_pos] = '?';
            search_prompt[search_pos+1] = ' ';
            search_prompt[search_pos+2] = 0;
        } else {
            kgl_putc('\007');
            hist_pos = hist_last;
        }
    }
}

static void
  search_addchar(c)
int  c;
{
    char *loc;

    search_update(c);
    if (c < 0) {
        if (search_pos > 0) {
            hist_pos = search_last;
        } else {
            kgl_buf[0] = 0;
            hist_pos = hist_last;
        }
        strcpy(kgl_buf, hist_buf[hist_pos]);
    }
    if ((loc = strstr(kgl_buf, search_string)) != 0) {
        kgl_fixup(search_prompt, 0, loc - kgl_buf);
    } else if (search_pos > 0) {
        if (search_forw_flg) {
            search_forw(0);
        } else {
            search_back(0);
        }
    } else {
        kgl_fixup(search_prompt, 0, 0);
    }
}

static void
  search_term()
{
    kgl_search_mode = 0;
    if (kgl_buf[0] == 0)         /* not found, reset hist list */
        hist_pos = hist_last;
    if (kgl_in_hook)
        kgl_in_hook(kgl_buf);
    kgl_fixup(kgl_prompt, 0, kgl_pos);
}

static void
  search_back(new_search)
int new_search;
{
    int    found = 0;
    char  *p, *loc;

    search_forw_flg = 0;
    if (kgl_search_mode == 0) {
        search_last = hist_pos = hist_last;
        search_update(0);
        kgl_search_mode = 1;
        kgl_buf[0] = 0;
        kgl_fixup(search_prompt, 0, 0);
    } else if (search_pos > 0) {
        while (!found) {
            p = hist_prev();
            if (*p == 0) {              /* not found, done looking */
               kgl_buf[0] = 0;
               kgl_fixup(search_prompt, 0, 0);
               found = 1;
            } else if ((loc = strstr(p, search_string)) != 0) {
               strcpy(kgl_buf, p);
               kgl_fixup(search_prompt, 0, loc - p);
               if (new_search)
                   search_last = hist_pos;
               found = 1;
            }
        }
    } else {
        kgl_putc('\007');
    }
}

static void
  search_forw(new_search)
int new_search;
{
    int    found = 0;
    char  *p, *loc;

    search_forw_flg = 1;
    if (kgl_search_mode == 0) {
        search_last = hist_pos = hist_last;
        search_update(0);
        kgl_search_mode = 1;
        kgl_buf[0] = 0;
        kgl_fixup(search_prompt, 0, 0);
    } else if (search_pos > 0) {
        while (!found) {
            p = hist_next();
            if (*p == 0) {              /* not found, done looking */
               kgl_buf[0] = 0;
               kgl_fixup(search_prompt, 0, 0);
               found = 1;
            } else if ((loc = strstr(p, search_string)) != 0) {
               strcpy(kgl_buf, p);
               kgl_fixup(search_prompt, 0, loc - p);
               if (new_search)
                   search_last = hist_pos;
               found = 1;
            }
        }
    } else {
        kgl_putc('\007');
    }
}
#endif
