/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef mowww_h
#define mowww_h

#define HAS_MOSAIC_IMG

#define mo_fail    0
#define mo_succeed 1
#define mo_status  int

#ifdef HAS_MOSAIC_IMG
/*For files img.c, picread.c, xpmread.c */

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct
{
  int default_width;                    
  int default_height;                   
  char *home_document;                  
  char *sendmail_command;
  char *print_command;
  int	cciPort;
  int  max_num_of_cci_connections;
  char *edit_command;
  Boolean edit_command_use_xterm;
  char *xterm_command;
  char *global_history_file;
  char *default_hotlist_file;
  char *private_annotation_directory;
  char *annotation_server;
  char *default_author_name;
  char *signature;

  char *http_proxy;
  char *ftp_proxy;
  char *wais_proxy;
  char *gopher_proxy;
  char *news_proxy;
  char *file_proxy;

  int colors_per_inlined_image;

  char *uncompress_command;
  char *gunzip_command;

  char *record_command_location;
  char *record_command;

  char *tmp_directory;

  char *full_hostname;

  int hdf_max_image_dimension;
  int hdf_max_displayed_datasets;
  int hdf_max_displayed_attributes;

  char *docs_directory;

  char *documents_menu_specfile;

  int max_wais_responses;

  char *global_extension_map;
  char *personal_extension_map;
  char *global_type_map;
  char *personal_type_map;

  int twirl_increment;

  int image_cache_size;

  char *default_font_choice;

  Boolean kiosk;
  Boolean kioskNoExit;
  Boolean use_global_history;           
  Boolean display_urls_not_titles;      
  Boolean confirm_exit;
  Boolean default_fancy_selections;
  Boolean annotations_on_top;
  Boolean track_visited_anchors;
  Boolean catch_prior_and_next;
  Boolean hdf_power_user;

  Boolean reload_reloads_images;
  Boolean reload_pragma_no_cache;
  Boolean simple_interface;
  Boolean delay_image_loads;

  Boolean use_default_extension_map;
  Boolean use_default_type_map;
  Boolean twirling_transfer_icon;
  Boolean protect_me_from_myself;

  Boolean reverse_inlined_bitmap_colors;

  Boolean confirm_delete_annotation;
  Boolean tweak_gopher_types;

  /* If True, we can't call gethostbyname to find out who we are. */
  Boolean gethostbyname_is_evil;

  Boolean auto_place_windows;
  Boolean initial_window_iconic;

  Boolean track_pointer_motion;
  Boolean track_full_url_names;

  Boolean print_header;
  Boolean print_header_top;
  Boolean print_header_left;

  char *mail_filter_command;

#ifdef __sgi
  Boolean debugging_malloc;
#endif
} AppData, *AppDataPtr;

#endif /*HAS_MOSAIC_IMG*/

#ifdef __cplusplus
extern "C"{
#endif
void      mo_set_globals                  (void*,void*,void(*)(char*));

char*     mo_pull_er_over                 (char*,char**);
char*     mo_url_canonicalize_keep_anchor (char*,char*);
char*     mo_convert_newlines_to_spaces   (char*);
char*     mo_url_extract_anchor           (char*);
char*     mo_url_canonicalize             (char*,char*);
void      mo_gui_done_with_icon           ();
int       mo_here_we_are_son              (char*);
char*     mo_tmpnam                       ();
mo_status mo_pull_er_over_virgin          (char *, char *);

#ifdef HAS_MOSAIC_IMG
mo_status      mo_cache_data                (char*, void*, int); 
void*          mo_fetch_cached_image_data   (char*);
mo_status      mo_register_image_resolution (void*);
unsigned char* ReadBitmap                   (char *, int *, int*, XColor*,int *);
void           MedianCut                    (unsigned char*,int*,int*,XColor*,int,int);
#endif /*HAS_MOSAIC_IMG*/

#ifdef __cplusplus
}
#endif

#endif  /*mowww_h*/
