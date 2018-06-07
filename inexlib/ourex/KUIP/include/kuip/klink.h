#ifndef kuip_klink_h
#define kuip_klink_h
/*
 * $Id: klink.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: klink.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.12  2009/06/30 12:26:52  barrand
 * *** empty log message ***
 *
 * Revision 1.11  2009/06/30 08:48:58  barrand
 * *** empty log message ***
 *
 * Revision 1.10  2009/06/29 16:27:16  barrand
 * *** empty log message ***
 *
 * Revision 1.9  2005/06/08 07:55:10  barrand
 * *** empty log message ***
 *
 * Revision 1.8  2004/12/23 14:52:36  barrand
 * *** empty log message ***
 *
 * Revision 1.7  2004/09/04 09:40:58  barrand
 * *** empty log message ***
 *
 * Revision 1.6  2004/09/03 14:33:47  barrand
 * *** empty log message ***
 *
 * Revision 1.5  2004/07/14 09:52:49  barrand
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/04 08:00:39  barrand
 * *** empty log message ***
 *
 * Revision 1.3  2003/09/10 09:25:27  barrand
 * *** empty log message ***
 *
 * Revision 1.2  1999/11/08 11:50:49  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25  13:17:43  barrand
 * KUIP
 *
 * Revision 1.2  1996/04/16 13:26:44  gunter
 * Mods to compile kuipc. The includes needed only for kuipc where moved
 *  to kuipc, together with m4 files to create/update these header files.
 *
 * Revision 1.1.1.1  1996/03/08 15:33:00  mclareni
 * Kuip
 *
 */
/* klink.h: demand linking of special routines */

#undef __ /*GB*/

#define __ /* Null */

#include "klink1.h"

#undef __


typedef struct {
  SUBROUTINE *user_exit_F;      /* set by KUEXIT */
  SUBROUTINE *user_quit_F;      /* set by KUQUIT */
  SUBROUTINE *user_break_F;     /* set by KUBREK */
  SUBROUTINE *user_edit_F;      /* set by KUEUSR */
  SUBROUTINE *user_comis_F;     /* set by KUCOMV */
  /*GB SUBROUTINE *user_sigma_F;*/     /* set by KUSIGM */
  SigmaFunc  *user_sigma_F;     /*GB*/
  SUBROUTINE *user_grfl_F;      /* set by KUGRFL */
  SUBROUTINE *user_term_F;      /* set by KUTERM */
  SUBROUTINE *user_input_F;     /* set by KUSER */
  SUBROUTINE *user_locate_F;    /* set by KUMLOC */
  /* indirect calls to avoid linking Motif                                   */
  IntFunc    *disp_panel_C;     /* display command panel (km_display_cmdpan) */
  IntFunc    *disp_kpanel_C;    /* display KUIP panel (km_display_kpanel)    */
  IntFunc    *close_kpanel_C;   /* close KUIP panel (km_close_kpanel)        */
  IntFunc    *disp_kmpanel_C;   /* add panel inside palette (km_panel_add)   */
  IntFunc    *disp_text_C;      /* display text widget (km_display_sctext)   */

  IntFunc    *disp_choice_C;    /* display a choice of commands (?)          */
  IntFunc    *disp_clean_C;     /* clean before action (km_destroy_all_popup)*/
  IntFunc    *disp_flush_C;     /* flush event queue (FlushEvents)           */
  IntFunc    *disp_busy_C;      /* show busy cursor (km_all_cursor)          */
  IntFunc    *disp_exit_C;      /* ask confirmation for exit                 */

  IntFunc    *disp_quit_C;      /* ask confirmation for exit                 */
  IntFunc    *disp_select_C;    /* select from a number of buttons           */
  IntFunc    *disp_cmd_list_C;  /* display list of commands (km_cmd_list)    */
  IntFunc    *disp_list_C;      /* display list of items (km_display_list)   */
  IntFunc    *callb_kmenu;      /* kuipList callback in KMENU (km_kmenu_OK)  */

  CharFunc   *disp_prompt_C;    /* prompt for input                          */
  CharFunc   *disp_passwd_C;    /* prompt for password input                 */
  /* indirect calls to avoid linking HIGZ without style G                    */
  IntFunc    *higz_init_C;      /* initialize menu mode */
  /*GB : vector functions. */
  IntFunc    *vector_create_C;
  IntFunc    *vector_delete_C;

  IntFunc    *vector_find_C;
  IntFunc    *vector_value_C;
  IntFunc    *vector_list_C;
  IntFunc    *vector_copy_C;
  IntFunc    *vector_input_C;

  IntFunc    *vector_print_C;
  IntFunc    *vector_read_C;
  IntFunc    *vector_write_C;
  IntFunc    *vector_operation_C;  
  /*GB : Getine :*/
  VoidFunc   *kgl_setwidth;

  VoidFunc   *kgl_config;
  CharFunc   *kgl_getline;
  VoidFunc   *kgl_histadd;
  /*GB : callback data :*/
  void       *action_data;
  void       *disp_flush_C_data;

  void       *disp_prompt_C_data;
  void       *disp_exit_C_data;
  void       *disp_quit_C_data;
  void       *vector_create_C_data;
  void       *vector_delete_C_data;

  void       *vector_find_C_data;
  void       *vector_value_C_data;
  void       *vector_list_C_data;
  void       *vector_copy_C_data;
  void       *vector_input_C_data;

  void       *vector_print_C_data;
  void       *vector_read_C_data;
  void       *vector_write_C_data;
  void       *vector_operation_C_data;
  void       *user_sigma_F_data;
} KcJmpAddr;
EXTERN KcJmpAddr kjmpaddr; /* Initialized in kuinit.c */

#undef StartQuotedInclude
#undef EndQuotedInclude

#endif //kuip_klink_h
