#include <stddef.h>

/*Cm*/
#ifdef HAS_CM
#include <CmMessage.h>
#endif

/*Co*/
#include <CString.h>

/*Xz*/
#include <HF77.h>
#include <HBook.h>

#include <XzCm.h>

/***************************************************************************/
int XzCmPutHistogram (
 void* a_message
,int   a_id
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_message==NULL)           return 0;

#ifdef HAS_CM
 {int                           id;
  char*                         title;
  int                           nx,ny;
  double                        xmi,xma,ymi,yma;
  id                            = a_id;

  CmMessagePutText              ((CmMessage)a_message,a_name);
  HBookGetHistogramInformations (id,&title,&nx,&xmi,&xma,&ny,&ymi,&yma);
  CmMessagePutText              ((CmMessage)a_message,title);
  CStringDelete                 (title);
  CmMessagePutLong              ((CmMessage)a_message,(long)id);
  CmMessagePutText              ((CmMessage)a_message,NULL); /*Monitoring mode.*/

  if(ny!=0)
    {  /* Histo 2D */
      int          nent;
      int          ix,iy;
      int          iax;
      float        mean,stddev;
      double       all;
      
      CmMessagePutInt    ((CmMessage)a_message,2);

      CmMessagePutInt    ((CmMessage)a_message,nx);
      CmMessagePutDouble ((CmMessage)a_message,xmi);
      CmMessagePutDouble ((CmMessage)a_message,xma);

      CmMessagePutInt    ((CmMessage)a_message,ny);
      CmMessagePutDouble ((CmMessage)a_message,ymi);
      CmMessagePutDouble ((CmMessage)a_message,yma);

      hnoent             (&id,&nent);
      CmMessagePutInt    ((CmMessage)a_message,nent);
/*Here we assume strongly that weight = 1. */
      all                = (double)nent;
      CmMessagePutDouble ((CmMessage)a_message,all);

      iax                = 0;
      ohstat             (&id ,&iax,&mean,&stddev);
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(mean));
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(stddev * stddev + mean * mean) );

      iax                = 1;
      ohstat             (&id ,&iax,&mean,&stddev);
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(mean));
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(stddev * stddev + mean * mean) );
      
      for(iy=1;iy<=ny;iy++)
	for(ix=1;ix<=nx;ix++)
	  { 
	    double             sw,sw2;
	    sw                 = (double)hij ( &id, &ix, &iy);
	    nent               = (int)sw; /*Assumed filled with weigth of 1.*/
	    sw2                = 0.;  /*Not handled for 2-dim histo.*/
	    CmMessagePutInt    ((CmMessage)a_message,nent);
	    CmMessagePutDouble ((CmMessage)a_message,sw);
	    CmMessagePutDouble ((CmMessage)a_message,sw2);
	  }


    }
  else
    {  /* Histo 1D */
      int          profile;
      int          nent;
      int          ix;
      int          iax;
      float        mean,stddev;
      float        under,over;
      double       all;
      
      profile            = ohisp (&id);  /*Is it a profile ?*/

      CmMessagePutInt    ((CmMessage)a_message,1);
      CmMessagePutInt    ((CmMessage)a_message,profile);

      CmMessagePutInt    ((CmMessage)a_message,nx);
      CmMessagePutDouble ((CmMessage)a_message,xmi);
      CmMessagePutDouble ((CmMessage)a_message,xma);

      hnoent             (&id,&nent);
      CmMessagePutInt    ((CmMessage)a_message,nent);
/*Here we assume strongly that weight = 1. */
      all                = (double)nent;
      CmMessagePutDouble ((CmMessage)a_message,all);
      
      iax                = -1;
      ohstat             (&id ,&iax,&mean,&stddev);
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(mean));
      CmMessagePutDouble ((CmMessage)a_message,all * (double)(stddev * stddev + mean * mean));

/*Underflow.*/
      ix                 = 0;
      under              = hi  ( &id, &ix);
      CmMessagePutInt    ((CmMessage)a_message,(int)under);

/*Overflow.*/
      ix                 = nx+1;
      over               = hi  ( &id, &ix);
      CmMessagePutInt    ((CmMessage)a_message,(int)over);
      
      if(profile==0)
	{
	  for(ix=1;ix<=nx;ix++)
	    { 
	      double             val,sw,sw2;
	      int                iy   = 0;
              int                func = 2;
	      sw                 = (double)hi(&id,&ix);
	      nent               = (int)sw; /*Assumed filled with weigth of 1.*/
	      val                = (double)hgcont (&id,&ix,&iy,&func);
	      sw2                = val * val;  /*Ok if HBARX has been called.*/
	      CmMessagePutInt    ((CmMessage)a_message,nent);
	      CmMessagePutDouble ((CmMessage)a_message,sw);
	      CmMessagePutDouble ((CmMessage)a_message,sw2);
	    }
	}
      else
	{
	  for(ix=1;ix<=nx;ix++)
	    { 
	      double             val,sw,sw2;
	      int                iy   = 0;
              int                func = 3;
	      val                = (double)hi          (&id,&ix);
	      nent               = (int)hgcont (&id,&ix,&iy,&func);
	      sw                 = val * ((double)nent);
	      func               = 2;
	      val                = (double)hgcont (&id,&ix,&iy,&func);
	      sw2                = val * val;  
	      CmMessagePutInt    ((CmMessage)a_message,nent);
	      CmMessagePutDouble ((CmMessage)a_message,sw);
	      CmMessagePutDouble ((CmMessage)a_message,sw2);
	    }
	}

    }

  return 1;
 }
#else  /*HAS_CM*/
  CWarn  ("Xz package not compiled with the HAS_CM option.\n");
  a_id   = 0;
  a_name = NULL;
  return 0;
#endif /*HAS_CM*/
}
