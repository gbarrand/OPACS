/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OColor_h
#define OColor_h
 
/* ocolormap_X colors */
typedef enum {
  OColorAquamarine = 0,
  OColorMediumaquamarine,
  OColorBlack,    /*2*/
  OColorBlue,     /*3*/
  OColorCadetblue,
  OColorCornflowerblue,
  OColorDarkslateblue,
  OColorLightblue,
  OColorLightsteelblue,
  OColorMediumblue,
  OColorMediumslateblue, /*10*/
  OColorMidnightblue,
  OColorNavyblue,
  OColorNavy,
  OColorSkyblue,
  OColorSlateblue,
  OColorSteelblue,
  OColorCoral,
  OColorCyan,      /*18*/
  OColorFirebrick,
  OColorBrown,     /*20*/
  OColorGold,
  OColorGoldenrod,
  OColorGreen,     /*23*/
  OColorDarkgreen,
  OColorDarkolivegreen,
  OColorForestgreen,
  OColorLimegreen,
  OColorMediumseagreen,
  OColorMediumspringgreen,
  OColorPalegreen, /*30*/
  OColorSeagreen,
  OColorSpringgreen,
  OColorYellowgreen,
  OColorDarkslategrey,
  OColorDimgrey,
  OColorLightgrey,
  OColorGrey,
  OColorKhaki,
  OColorMagenta, /*39*/
  OColorMaroon,  /*40*/
  OColorOrange,
  OColorOrchid,
  OColorDarkorchid,
  OColorMediumorchid,
  OColorPink,
  OColorPlum,
  OColorRed,    /*47*/
  OColorIndianred,
  OColorMediumvioletred,
  OColorOrangered, /*50*/
  OColorVioletred,
  OColorSalmon,
  OColorSienna,
  OColorTan,
  OColorThistle,
  OColorTurquoise,   
  OColorDarkturquoise,
  OColorMediumturquoise,
  OColorViolet,
  OColorBlueviolet,  /*60*/
  OColorWheat,
  OColorWhite,       /*62*/
  OColorYellow,      /*63*/
  OColorGreenyellow  /*64*/
} OColor;

#ifdef __cplusplus
extern "C"{
#endif
char**        OColorGetNames      (int*);
int           OColorGetIdentifier (char*);
char*         OColorGetName       (OColor);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOColor(0); /**/
#endif

#endif  /*OColor_h*/



