/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HF77_h
#define HF77_h

#ifdef __CI_SPECIFIC__
#define HAS_NOT_U
#endif

/* C calling f77 */
#ifndef HAS_NOT_U
/* KERNLIB */
#define uctoh    uctoh_
/* HBOOK */
#define hidall   hidall_
#define hnoent   hnoent_
#define hdelet   hdelet_
#define hi       hi_
#define hij      hij_
#define hgn      hgn_
#define hrin     hrin_
#define hlimit   hlimit_
#define hexist   hexist_
#define hxi      hxi_
#define hix      hix_
#define hie      hie_
#define hprint   hprint_
#define hrfile   hrfile_
#define hcreateg hcreateg_
#define hmapg    hmapg_
#define hfreeg   hfreeg_
#define hmapm    hmapm_
#define hfreem   hfreem_
#define hfill    hfill_
#define hfn      hfn_
#define hfitga   hfitga_
#define hfitex   hfitex_
#define hfitpo   hfitpo_
#define hgcont   hgcont_
#define hminim   hminim_
#define hmaxim   hmaxim_
#define hbarx    hbarx_
/* HPLOT */
#define hplint  hplint_
#define hplnt   hplnt_
#define hplego  hplego_
#define hplgiv  hplgiv_
/* HIGZ */
#define iclwk   iclwk_
#define iopwk   iopwk_
#define iacwk   iacwk_
#define idawk   idawk_
#define iclrwk  iclrwk_
#define iuwk    iuwk_
#define igrsiz  igrsiz_
#define iscr    iscr_
#define ixrscwi ixrscwi_
#define ixq2buf ixq2buf_
#define ixs2buf ixs2buf_
#define ixsavwi ixsavwi_
#define ixselwi ixselwi_
#define ixupdwi ixupdwi_
#define ixsetwi ixsetwi_
#define ixopnpx ixopnpx_
#define ixclpx  ixclpx_
/* ohbook.f */
#define opawi  opawi_
#define ohraz  ohraz_ 
#define ohinit ohinit_
#define ohopen ohopen_
#define ohrout ohrout_
#define ohrend ohrend_
#define ohlnxt ohlnxt_
#define ohpwd  ohpwd_
#define ohcd   ohcd_
#define ohlsr  ohlsr_
#define ohgivh ohgivh_
#define ohgivn ohgivn_
#define ohtype ohtype_
#define ohtit  ohtit_
#define ohstat ohstat_
#define ohgmap ohgmap_
#define ohgr   ohgr_
#define ohgunm ohgunm_
#define ohmmap ohmmap_
#define ohmr   ohmr_
#define ohmunm ohmunm_
#define ohbk1  ohbk1_
#define ohbk2  ohbk2_
#define ohbkp  ohbkp_
#define ohbkn  ohbkn_
#define ohisp  ohisp_
#define ohidop ohidop_
#define ohpax  ohpax_
/* ohplot.f */
#define ohplot  ohplot_
#define ohpset  ohpset_
#define ohpopt  ohpopt_
#define ohptit  ohptit_
#define ohpsiz  ohpsiz_
#define oiswk   oiswk_
#define oigwid  oigwid_
#define oisrat  oisrat_
#define ohpzon  ohpzon_
#define ohpax   ohpax_
#endif

#ifdef __cplusplus
extern "C"{
#endif
/* KERNLIB */
void uctoh     (char*,int*,int*,int*);
/* HBOOK */
void  hidall   (int*, int*);
void  hnoent   (int*, int*);
void  hdelet   (int*);
float hi       (int*, int*);
float hij      (int*, int*, int*);
void  hgn      (int*, int*, int*, float*, int*);
void  hrin     (int*,int*,int*);
void  hlimit   (int*);
int   hexist   (int*);
void  hxi      (int*,float*,int*);
void  hix      (int*,int*,float*);
float hie      (int*,int*);
void  hprint   (int*);
void  hrfile   (int*,void*,void*);
int   hmapg    (void*,int*,int*);
int   hfreeg   (int*,int*,int*);
int   hmapm    (void*,int*,int*);
int   hfreem   (int*);
void  hfill    (int*,float*,float*,float*);
void  hfn      (int*,float*);
void  hfitga   (int*,float*,float*,float*,float*,int*,float*);
void  hfitex   (int*,float*,float*,float*,int*,float*);
void  hfitpo   (int*,int*,float*,float*,int*,float*);
float hgcont   (int*,int*,int*,int*);
void  hminim   (int*,float*);
void  hmaxim   (int*,float*);
void  hbarx    (int*);
/* HPLOT */
void hplint  (int*);
void hplnt   (int*,int*,char**,int*,int*,int*,int*);
void hplego  (int*,float*,float*);
void hplgiv  (float*,float*,float*,float*);
/* HIGZ */
void iclwk   (int*);
void iopwk   (int*,int*,int*);
void iacwk   (int*);
void idawk   (int*);
void iclrwk  (int*,int*);
void iuwk    (int*,int*);
void igrsiz  (int*);
void iscr    (int*,int*,float*,float*,float*);
#ifdef IXHIGZ
void ixrscwi (int*,int*,int*);
void ixsdswi (Display*,Window);
/*
void ixgetwi (int,Window*);
*/
int  ixq2buf (int*);
void ixs2buf (int*,int*);
void ixsavwi ();
void ixselwi (int*);
void ixupdwi (int*);
void ixsetwi ();
int  ixopnpx (int*,int*);
void ixclpx  ();
#endif
/* ohbook.f */
void  opawi  ();
void  ohbk1  (int*,void*,int*,int*,float*,float*,float*);
void  ohbk2  (int*,void*,int*,int*,float*,float*,int*,float*,float*,float*);
void  ohbkp  (int*,void*,int*,int*,float*,float*,float*,float*,void*,int*);
void  ohbkn  (int*,void*,int*,int*,int*,void*,int*);
void  ohraz  (int*);
void  ohrout (int*);
void  ohinit (int*);
int   ohopen (int*,void*,int*,void*,int*,int*);
void  ohrend (int*,void*,int*);
void  ohlnxt (int*,void*,int*,void*,int*,void*,int*);
void  ohpwd  (void*,int*);
int   ohcd   (void*,int*);
void  ohlsr  ();
void  ohgivh (int*,void*,int*,int*,float*,float*,int*,float*,float*);
void  ohgivn (int*,void*,int*,int*,int*,void*,int*);
int   ohtype (int*);
void  ohtit  (void*,int*);
void  ohstat (int*,int*,float*,float*);
int   ohgmap (void*,int*,int*);
void  ohgr   (void*,int*,int*);
void  ohgunm (int*,int*);
int   ohmmap (void*,int*);
void  ohmr   (void*,int*,int*);
void  ohmunm (int*);
int   ohisp  (int*);
void  ohidop (int*,void*,int*);
/* ohplot.f */
void ohplot  (int*,void*,int*);
void ohpset  (void*,int*,float*);
void ohpopt  (void*,int*);
void ohptit  (void*,int*);
void ohpsiz  (float*,float*);
void oiswk   (int*,int*,int*);
void oigwid  (int*,int*);
void oisrat  (int*,int*);
void ohpzon  (int*,int*,int*,void*,int*);
void ohpax   (void*,int*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindHF77(0); /**/
#endif


#endif  /*HF77_h*/
