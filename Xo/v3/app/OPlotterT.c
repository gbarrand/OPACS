/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CMath.h>
#include <CList.h>
#include <OShell.h>
#include <OType.h>

#include <CoCommands.h>
#include <GoCommands.h>

#include <OHistogram.h>
#include <HoTypes.h>

#include <OColormap.h>
#include <OCamera.h>

#include <OPlotter.h>

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         count;
  OHistogram  hist;
  OHistogram* hists = NULL;
  OCamera     camera;
  OPlotter    plot;
  ONode       goplot;
/*.........................................................................*/
  HoSetTypes ();

 {OShell        osh;
  osh           = OShellCreate ("osh");
  CoAddCommands (osh);
  GoAddCommands (osh);
  OShellExecute (osh,"load X11.odb");}

  hist          = OHistogramCreate ("mon_histo",1,"x",100,-5.,5.);
  OHistogramSetTitle (hist,"Hello");
  for(count=0;count<10000;count++)
    {
      double            value;
      value             = CMathGetRandomGauss();
      OHistogramFillOneDimensional  (hist,value,1.);
    }

  OHistogramDump                 (hist);

  camera                         = OCameraCreate ("my_camera",300,300);
  OCameraSetColormap             (camera,OColormapGetIdentifier("ocolormap_X"));
  OCameraSetBackgroundColorIndex (camera,OColorLightgrey);

  plot                           = OPlotterCreate ("mon_plot");
  goplot                         = (ONode)OPlotterGetRootNode (plot);
  OCameraViewNode                (camera,goplot);
  /*
  OPlotterSetBinsContext         (plot,"color brown lineStyle solid areaStyle solid  markerStyle cross  modeling solid painting shadow");
  */

  CListAddEntry          ((CList*)&hists,(CEntry)hist);
  OPlotterRepresent      (plot,(CList)hists,NULL,NULL);
  CListDelete            ((CList)hists);

 {OLayout                layout;
  layout                 = OPlotterGetLayout(plot);
  OCameraSetAspectRatio  (camera,0);
  OCameraSetPlotLayout   (camera,(layout==OLayoutXYV) || (layout==OLayoutXYZ) ? 1 : 0);}

/*Produce a PostScript file.*/
  OCameraProducePS_File  (camera,"out.ps");

  OPlotterDelete         (plot);
  OPlotterClearClass     ();
  OTypeClearClass        ();
  
  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}






