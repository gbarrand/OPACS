/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

/* This document should be included in whatever source document
   sets up the Intrinsics.  It is in a separate file so it doesn't
   clutter up that file.  So sue me. */

#ifndef __MOSAIC_XRESOURCES_H__

/* ----------------------------- X Resources ------------------------------ */
#define offset(x) XtOffset (AppDataPtr, x)

static XtResource resources[] = {
  /* default font choice from Options menu choices */
  { "defaultFontChoice", "DefaultFontChoice", XtRString, sizeof (char *),
      offset (default_font_choice), XtRString, "TimesRegular" },

  /* Whether Mosaic reads and writes global history from 
     ~/.mosaic-global-history
     and thus provides persistent history tracking. */
  { "kiosk", "Kiosk", XtRBoolean, sizeof (Boolean), 
      offset (kiosk), XtRString, "False" },
  { "kioskNoExit", "KioskNoExit", XtRBoolean, sizeof (Boolean),
      offset (kioskNoExit), XtRString, "False" },
  { "useGlobalHistory", "UseGlobalHistory", XtRBoolean, sizeof (Boolean),
      offset (use_global_history), XtRString, "True" },
  /* Whether titles will be displayed wherever URL\'s are normally
     displayed. */
  { "displayURLsNotTitles", "DisplayURLsNotTitles", XtRBoolean, 
      sizeof (Boolean),
      offset (display_urls_not_titles), XtRString, "False" },
  /* Default width for a Document View window.  This will change as windows
     are cloned. */
  { "defaultWidth", "DefaultWidth", XtRInt, sizeof (int),
      offset (default_width), XtRString, "640" },
  /* Default height for a Document View window. */
  { "defaultHeight", "DefaultHeight", XtRInt, sizeof (int),
      offset (default_height), XtRString, "700" },
  /* Startup document. */
  { "homeDocument", "HomeDocument", XtRString, sizeof (char *),
      offset (home_document), XtRString, 
      HOME_PAGE_DEFAULT },
  { "confirmExit", "ConfirmExit", XtRBoolean, sizeof (Boolean),
      offset (confirm_exit), XtRString, "True" },
  /* THIS USED TO BE mailCommand BUT IS NOW sendmailCommand. */
  { "sendmailCommand", "SendmailCommand", XtRString, sizeof (char *),
      offset (sendmail_command), XtRString, "/usr/lib/sendmail" },
  /* Ignore this.  Stealth feature. */
  { "mailFilterCommand", "MailFilterCommand", XtRString, sizeof (char *),
      offset (mail_filter_command), XtRString, NULL },
  { "printCommand", "PrintCommand", XtRString, sizeof (char *),
      offset (print_command), XtRString, "lpr" },
  { "cciPort","CCIPort",XtRInt,sizeof (int),
      offset (cciPort), XtRString, "0" },
  { "maxNumCCIConnect","MaxNumCCIConnect",XtRInt,sizeof (int),
      offset (max_num_of_cci_connections), XtRString, "1" },
  { "editCommand", "EditCommand", XtRString, sizeof (char *),
      offset (edit_command), XtRString, NULL },
  { "editCommandUseXterm", "EditCommandUseXterm", XtRBoolean, sizeof (Boolean),
      offset (edit_command_use_xterm), XtRString, "True" },
  { "xtermCommand", "XtermCommand", XtRString, sizeof (char *),
      offset (xterm_command), XtRString, 
#ifdef _AIX
      "aixterm -v",
#else /* not _AIX */
      "xterm",
#endif /* not _AIX */
    },
  { "globalHistoryFile", "GlobalHistoryFile", XtRString, 
      sizeof (char *),
      offset (global_history_file), XtRString, ".mosaic-global-history" },
  { "defaultHotlistFile", "DefaultHotlistFile", XtRString, 
      sizeof (char *),
      offset (default_hotlist_file), XtRString, ".mosaic-hotlist-default" },
  { "personalAnnotationDirectory", "PersonalAnnotationDirectory", XtRString, 
      sizeof (char *),
      offset (private_annotation_directory), XtRString, 
      ".mosaic-personal-annotations" },
  /* Whether selections should be fancy, by default. */
  { "fancySelections", "FancySelections", XtRBoolean,
      sizeof (Boolean), offset (default_fancy_selections), 
      XtRString, "False" },
  { "defaultAuthorName", "DefaultAuthorName", XtRString, sizeof (char *),
      offset (default_author_name), XtRString, NULL },
  { "signature", "Signature", XtRString, sizeof (char *),
      offset (signature), XtRString, NULL },
  { "annotationsOnTop", "AnnotationsOnTop", XtRBoolean, sizeof (Boolean),
      offset (annotations_on_top), XtRString, "False" },

  { "colorsPerInlinedImage", "ColorsPerInlinedImage", XtRInt, sizeof (int),
      offset (colors_per_inlined_image), XtRString, "50" },
  { "trackVisitedAnchors", "TrackVisitedAnchors", XtRBoolean, sizeof (Boolean),
      offset (track_visited_anchors), XtRString, "True" },

  { "uncompressCommand", "UncompressCommand", XtRString, sizeof (char *), 
      offset (uncompress_command), XtRString, "uncompress" },
  { "gunzipCommand", "GunzipCommand", XtRString, 
      sizeof (char *), offset (gunzip_command), XtRString, "gunzip -f -n" },

  { "recordCommandLocation", "RecordCommandLocation", XtRString, 
      sizeof (char *), offset (record_command_location), XtRString,
#ifdef __hpux
      "/usr/audio/bin/srecorder" },
#else
#if defined(__sgi)
      "/usr/sbin/recordaiff" },
#else
#if defined (sun)
      "/usr/demo/SOUND/record" },
#else
      "/bin/true" },
#endif /* if sun */
#endif /* if sgi */
#endif /* ifdef */
  { "recordCommand", "RecordCommand", XtRString, sizeof (char *),
      offset (record_command), XtRString,
#ifdef __hpux
      "srecorder -au" },
#else
#if defined(__sgi)
      "recordaiff -n 1 -s 8 -r 8000" },
#else
#if defined (sun)
      "record" },
#else
      "true" },
#endif /* if sun */
#endif /* if sgi */
#endif /* ifdef */

  { "gethostbynameIsEvil", "GethostbynameIsEvil", XtRBoolean, sizeof (Boolean),
      offset (gethostbyname_is_evil), XtRString, "False" },
  { "autoPlaceWindows", "AutoPlaceWindows", XtRBoolean, sizeof (Boolean),
      offset (auto_place_windows), XtRString, "True" },
  { "initialWindowIconic", "InitialWindowIconic", XtRBoolean, sizeof (Boolean),
      offset (initial_window_iconic), XtRString, "False" },

  { "tmpDirectory", "TmpDirectory", XtRString, sizeof (char *),
      offset (tmp_directory), XtRString, NULL },
  { "annotationServer", "AnnotationServer", XtRString, sizeof (char *),
      offset (annotation_server), XtRString, NULL },
  { "catchPriorAndNext", "CatchPriorAndNext", XtRBoolean, sizeof (Boolean),
      offset (catch_prior_and_next), XtRString, "False" },

  /* Proxy gateway resources */
  { "httpProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (http_proxy), XtRString, NULL },
  { "ftpProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (ftp_proxy), XtRString, NULL },
  { "waisProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (wais_proxy), XtRString, NULL },
  { "gopherProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (gopher_proxy), XtRString, NULL },
  { "newsProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (news_proxy), XtRString, NULL },
  { "fileProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (file_proxy), XtRString, NULL },

  { "fullHostname", "FullHostname", XtRString, sizeof (char *),
      offset (full_hostname), XtRString, NULL },
  { "reverseInlinedBitmapColors", "ReverseInlinedBitmapColors", XtRBoolean,
      sizeof (Boolean),
      offset (reverse_inlined_bitmap_colors), XtRString, "False" },
  { "confirmDeleteAnnotation", "ConfirmDeleteAnnotation", 
      XtRBoolean, sizeof (Boolean),
      offset (confirm_delete_annotation), XtRString, "True"},
  { "tweakGopherTypes", "TweakGopherTypes", XtRBoolean, sizeof (Boolean),
      offset (tweak_gopher_types), XtRString, "True" },

  /* --- new in 2.0 --- */
  { "trackPointerMotion", "TrackPointerMotion", XtRBoolean, sizeof (Boolean),
      offset (track_pointer_motion), XtRString, "True" },
  { "trackFullURLs", "TrackFullURLs", XtRBoolean, sizeof (Boolean),
      offset (track_full_url_names), XtRString, "True" },

  { "hdfMaxImageDimension", "HdfMaxImageDimension", XtRInt, sizeof (int),
      offset (hdf_max_image_dimension), XtRString, "400" },
  { "hdfMaxDisplayedDatasets", "HdfMaxDisplayedDatasets", XtRInt, sizeof (int),
      offset (hdf_max_displayed_datasets), XtRString, "15" },
  { "hdfMaxDisplayedAttributes", "HdfMaxDisplayedAttributes", XtRInt, sizeof (int),
      offset (hdf_max_displayed_attributes), XtRString, "10" },
  { "hdfPowerUser", "HdfPowerUser", XtRBoolean, sizeof (Boolean),
      offset (hdf_power_user), XtRString, "False" },

  { "docsDirectory", "DocsDirectory", XtRString, sizeof (char *),
      offset (docs_directory), XtRString, NULL },

  { "documentsMenuSpecfile", "DocumentsMenuSpecfile", XtRString, sizeof (char *),
      offset (documents_menu_specfile), XtRString, 
      "/usr/local/lib/mosaic/documents.menu" },

  { "reloadReloadsImages", "ReloadReloadsImages", XtRBoolean, sizeof (Boolean),
      offset (reload_reloads_images), XtRString, "False" },
  { "reloadPragmaNoCache", "ReloadPragmaNoCache", XtRBoolean, sizeof (Boolean),
      offset (reload_pragma_no_cache), XtRString, "False" },
  { "simpleInterface", "SimpleInterface", XtRBoolean, sizeof (Boolean),
      offset (simple_interface), XtRString, "False" },

  { "maxWaisResponses", "MaxWaisResponses", XtRInt, sizeof (int),
      offset (max_wais_responses), XtRString, "200" },
  { "delayImageLoads", "DelayImageLoads", XtRBoolean, sizeof (Boolean),
      offset (delay_image_loads), XtRString, "False" },

  { "useDefaultExtensionMap", "UseDefaultExtensionMap", 
      XtRBoolean, sizeof (Boolean),
      offset (use_default_extension_map), XtRString, "True" },
  { "globalExtensionMap", "GlobalExtensionMap", 
      XtRString, sizeof (char *),
      offset (global_extension_map), XtRString, GLOBAL_EXTENSION_MAP },
  { "personalExtensionMap", "PersonalExtensionMap", 
      XtRString, sizeof (char *),
      offset (personal_extension_map), XtRString, ".mime.types" },

  { "useDefaultTypeMap", "UseDefaultTypeMap", 
      XtRBoolean, sizeof (Boolean),
      offset (use_default_type_map), XtRString, "True" },
  { "globalTypeMap", "GlobalTypeMap", 
      XtRString, sizeof (char *),
      offset (global_type_map), XtRString, GLOBAL_TYPE_MAP },
  { "personalTypeMap", "PersonalTypeMap", 
      XtRString, sizeof (char *),
      offset (personal_type_map), XtRString, ".mailcap" },

  { "twirlingTransferIcon", "TwirlingTransferIcon", 
      XtRBoolean, sizeof (Boolean),
      offset (twirling_transfer_icon), XtRString, "True" },
  { "twirlIncrement", "TwirlIncrement", XtRInt, sizeof (int),
      offset (twirl_increment), XtRString, "4096" },

  { "imageCacheSize", "ImageCacheSize", XtRInt, sizeof (int),
      offset (image_cache_size), XtRString, "2048" },

  { "protectMeFromMyself", "ProtectMeFromMyself", 
      XtRBoolean, sizeof (Boolean),
      offset (protect_me_from_myself), XtRString, "False" },

  { "printHeader", "PrintHeader",
      XtRBoolean, sizeof (Boolean),
      offset (print_header), XtRString, "True" },

  { "printHeaderTop", "PrintHeaderTop",
      XtRBoolean, sizeof (Boolean),
      offset (print_header_top), XtRString, "True" },

  { "printHeaderLeft", "PrintHeaderLeft",
      XtRBoolean, sizeof (Boolean),
      offset (print_header_left), XtRString, "False" },

#ifdef __sgi
  { "debuggingMalloc", "DebuggingMalloc", XtRBoolean, sizeof (Boolean),
      offset (debugging_malloc), XtRString, "False" },
#endif
};

#undef offset

#define __MOSAIC_XRESOURCES_H__
#endif /* __MOSAIC_XRESOURCES_H__ */
