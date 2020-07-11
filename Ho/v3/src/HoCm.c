/*
#define DEBUG
*/

#include <stddef.h>
#include <string.h>

/*Cm*/
#ifdef HAS_CM
#include <CmMessage.h>
#endif

/*Ho*/
#include <OHistogram.h>

#include <HoCm.h>
/***************************************************************************/
int HoCmPutHistogram (
 void* a_message
,OHistogram This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_message==NULL) return 0;

#ifdef HAS_CM
  CmMessagePutText  ((CmMessage)a_message,OHistogramGetName(This));
  CmMessagePutText  ((CmMessage)a_message,OHistogramGetTitle(This));
  CmMessagePutLong  ((CmMessage)a_message,(long)OHistogramGetUserData(This));
  CmMessagePutText  ((CmMessage)a_message,OHistogramGetMonitoringMode(This));

  if(OHistogramGetDimension(This)==2)
    { 
      int          nx,ny;
      double       xmi,xma,ymi,yma;
      int          ix,iy;
	  
      CmMessagePutInt    ((CmMessage)a_message,2);
	  
      OHistogramGetIthAxisAttributesF (This,OAxisX,"nmx",&nx,&xmi,&xma);
      CmMessagePutInt    ((CmMessage)a_message,nx);
      CmMessagePutDouble ((CmMessage)a_message,xmi);
      CmMessagePutDouble ((CmMessage)a_message,xma);

      OHistogramGetIthAxisAttributesF (This,OAxisY,"nmx",&ny,&ymi,&yma);
      CmMessagePutInt    ((CmMessage)a_message,ny);
      CmMessagePutDouble ((CmMessage)a_message,ymi);
      CmMessagePutDouble ((CmMessage)a_message,yma);

      CmMessagePutInt    ((CmMessage)a_message,OHistogramGetNumberOfEntries(This));
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetSumOfWeights(This));

      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedCoordinates(This,OAxisX));
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedSquaredCoordinates(This,OAxisX));
      
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedCoordinates(This,OAxisY));
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedSquaredCoordinates(This,OAxisY));
      
      for(iy=0;iy<ny;iy++)
	{
	  for(ix=0;ix<nx;ix++)
	    { 
	      CmMessagePutInt    ((CmMessage)a_message,OHistogramGetBinNumberOfEntries(This,ix,iy));
	      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetBinSumOfWeights(This,ix,iy));
	      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetBinSumOfSquaredWeights(This,ix,iy));
	    }
	}
    }
  else
    { 
      int          nx;
      double       xmi,xma;
      int          ix;
      
      CmMessagePutInt    ((CmMessage)a_message,1);
      CmMessagePutInt    ((CmMessage)a_message,OHistogramIsProfile(This));

      OHistogramGetIthAxisAttributesF (This,OAxisX,"nmx",&nx,&xmi,&xma);
      CmMessagePutInt    ((CmMessage)a_message,nx);
      CmMessagePutDouble ((CmMessage)a_message,xmi);
      CmMessagePutDouble ((CmMessage)a_message,xma);
	  
      CmMessagePutInt    ((CmMessage)a_message,OHistogramGetNumberOfEntries(This));
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetSumOfWeights(This));
	  
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedCoordinates(This,OAxisX));
      CmMessagePutDouble ((CmMessage)a_message,OHistogramGetIthAxisSumOfWeightedSquaredCoordinates(This,OAxisX));
      
      CmMessagePutInt    ((CmMessage)a_message,OHistogramGetIthAxisUnderflow(This,OAxisX));
      CmMessagePutInt    ((CmMessage)a_message,OHistogramGetIthAxisOverflow(This,OAxisX));
      
      for(ix=0;ix<nx;ix++)
	{ 
	  CmMessagePutInt    ((CmMessage)a_message,OHistogramGetBinNumberOfEntries(This,ix));
	  CmMessagePutDouble ((CmMessage)a_message,OHistogramGetBinSumOfWeights(This,ix));
	  CmMessagePutDouble ((CmMessage)a_message,OHistogramGetBinSumOfSquaredWeights(This,ix));
	}
	  
    }

  return 1;
#else  /*HAS_CM*/
  CWarn  ("Ho package not compiled with the HAS_CM option.\n");
  This   = NULL;
  return 0;
#endif /*HAS_CM*/
}
/***************************************************************************/
OHistogram HoCmGetHistogram (
 void* a_message
,int a_create
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_message==NULL) return NULL;
#ifdef HAS_CM
 {char*          name;
  char*          title;
  char*          monitoringMode;
  int            dimension;
  long           userData;
  OHistogram     hist = NULL;

  name           = CmMessageGetText  ((CmMessage)a_message);
  title          = CmMessageGetText  ((CmMessage)a_message);
  userData       = CmMessageGetLong  ((CmMessage)a_message);
  monitoringMode = CmMessageGetText  ((CmMessage)a_message);
  dimension      = CmMessageGetInt   ((CmMessage)a_message);

  hist           = OHistogramGetIdentifier (name);
  if(a_create==1) 
    {
      OHistogramDelete (hist);
      hist             = NULL;
    }

  if(dimension==2)
    {  /* Histo 2D */
      int    nx,ny;
      double xmi,xma,ymi,yma;
      int    nent;
      double all;
      int    ix,iy;
      double xsw,xsw2;
      double ysw,ysw2;

      nx      = CmMessageGetInt    ((CmMessage)a_message);
      xmi     = CmMessageGetDouble ((CmMessage)a_message);
      xma     = CmMessageGetDouble ((CmMessage)a_message);

      ny      = CmMessageGetInt    ((CmMessage)a_message);
      ymi     = CmMessageGetDouble ((CmMessage)a_message);
      yma     = CmMessageGetDouble ((CmMessage)a_message);

      if(hist==NULL) hist = OHistogramCreate   (name,2,"",nx,xmi,xma,"",ny,ymi,yma);

      nent    = CmMessageGetInt    ((CmMessage)a_message);
      all     = CmMessageGetDouble ((CmMessage)a_message);

      xsw     = CmMessageGetDouble ((CmMessage)a_message);
      xsw2    = CmMessageGetDouble ((CmMessage)a_message);

      ysw     = CmMessageGetDouble ((CmMessage)a_message);
      ysw2    = CmMessageGetDouble ((CmMessage)a_message);

      OHistogramSetTitle            (hist,title);
      OHistogramSetUserData         (hist,(void*)userData);
      OHistogramSetMonitoringMode   (hist,monitoringMode);
      OHistogramSetNumberOfEntries  (hist,nent);
      OHistogramSetSumOfWeights     (hist,all);

      OHistogramSetIthAxisSumOfWeightedCoordinates        (hist,OAxisX,xsw);
      OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (hist,OAxisX,xsw2);
      OHistogramSetIthAxisSumOfWeightedCoordinates        (hist,OAxisY,ysw);
      OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (hist,OAxisY,ysw2);

      for(iy=0;iy<ny;iy++)
	{
	  for(ix=0;ix<nx;ix++)
	    { 
	      OHistogramSetBinNumberOfEntries     (hist,ix,iy,CmMessageGetInt((CmMessage)a_message));
	      OHistogramSetBinSumOfWeights        (hist,ix,iy,CmMessageGetDouble((CmMessage)a_message));
	      OHistogramSetBinSumOfSquaredWeights (hist,ix,iy,CmMessageGetDouble((CmMessage)a_message));
	    }
	}
	  
    }
  else
    {  /* Histo 1D */
      int    profile;
      int    nx;
      double xmi,xma;
      int    nent;
      double all;
      int    ix;
      double xsw,xsw2;
      int    under,over;
      
      profile = CmMessageGetInt    ((CmMessage)a_message);
      nx      = CmMessageGetInt    ((CmMessage)a_message);
      xmi     = CmMessageGetDouble ((CmMessage)a_message);
      xma     = CmMessageGetDouble ((CmMessage)a_message);

      if(hist==NULL) hist = OHistogramCreate (name,1,"",nx,xmi,xma);

      nent   = CmMessageGetInt    ((CmMessage)a_message);
      all    = CmMessageGetDouble ((CmMessage)a_message);

      xsw    = CmMessageGetDouble ((CmMessage)a_message);
      xsw2   = CmMessageGetDouble ((CmMessage)a_message);

      under  = CmMessageGetInt    ((CmMessage)a_message);
      over   = CmMessageGetInt    ((CmMessage)a_message);

      OHistogramSetProfile             (hist,profile);
      OHistogramSetTitle               (hist,title);
      OHistogramSetUserData            (hist,(void*)userData);
      OHistogramSetMonitoringMode      (hist,monitoringMode);
      OHistogramSetNumberOfEntries     (hist,nent);
      OHistogramSetSumOfWeights        (hist,all);

      OHistogramSetIthAxisSumOfWeightedCoordinates         (hist,OAxisX,xsw);
      OHistogramSetIthAxisSumOfWeightedSquaredCoordinates  (hist,OAxisX,xsw2);
      OHistogramSetIthAxisUnderflow    (hist,OAxisX,under);
      OHistogramSetIthAxisOverflow     (hist,OAxisX,over);

      for(ix=0;ix<nx;ix++)
	{ 
	  OHistogramSetBinNumberOfEntries     (hist,ix,CmMessageGetInt((CmMessage)a_message));
	  OHistogramSetBinSumOfWeights        (hist,ix,CmMessageGetDouble ((CmMessage)a_message));
	  OHistogramSetBinSumOfSquaredWeights (hist,ix,CmMessageGetDouble ((CmMessage)a_message));
	}

    }

  return hist;
  }
#else  /*HAS_CM*/
  CWarn    ("Ho package not compiled with the HAS_CM option.\n");
  a_create = 0;
  return   NULL;
#endif /*HAS_CM*/
}
