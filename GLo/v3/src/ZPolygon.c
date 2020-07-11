/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stddef.h>

#include <CMemory.h>

#include <ZLineP.h>
#include <ZPointP.h>
#include <ZPolygon.h>

#define NUMPTSTOBUFFER 200
typedef struct _POINTBLOCK {
    ZPointRecord pts[NUMPTSTOBUFFER];
    struct _POINTBLOCK* next;
} POINTBLOCK;

#ifdef __cplusplus
extern "C"{
#endif
static void InsertEdgeInET (EdgeTable*,EdgeTableEntry*,int,ScanLineListBlock**,int*);
static void CreateETandAET ( int, ZPointRecord*,EdgeTable*,EdgeTableEntry*, EdgeTableEntry*,ScanLineListBlock*);
static void LoadAET        ( EdgeTableEntry*, EdgeTableEntry*);
static void ComputeWAET    ( EdgeTableEntry*);
static int  InsertAndSort  ( EdgeTableEntry*);
static void FreeStorage    ( ScanLineListBlock*);
static void ScanPoints     ( int, int,POINTBLOCK*,ZPolygonScanFunction);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b) ((a)<(b)?a:b)
#define MAXIMUM(a,b) ((a)>(b)?a:b)

static struct 
{
  int             pETEn;
  EdgeTableEntry* pETEs;
  int             numAllocPtBlocks;
  POINTBLOCK      FirstPtBlock;
} Class = {0,NULL,0};
/***************************************************************************/
void ZPolygonClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  POINTBLOCK* curPtBlock;
/*.........................................................................*/
  CMemoryFreeBlock    (Class.pETEs);
  Class.pETEs = NULL;
  Class.pETEn = 0;

  for (curPtBlock = Class.FirstPtBlock.next; --Class.numAllocPtBlocks >= 0;) 
    { 
      POINTBLOCK* tmpPtBlock;
      tmpPtBlock  = curPtBlock->next;
      CMemoryFreeBlock    (curPtBlock);
      curPtBlock  = tmpPtBlock;
    }
  Class.numAllocPtBlocks = 0;
}
/***************************************************************************/
void ZPolygonScan (
 int    Count               /* number of pts  */
,ZPointRecordList Pts	    /* the pts        */
,int	rule	            /* winding rule   */
,ZPolygonScanFunction a_proc
)
/***************************************************************************/
/*
 *     polytoregion
 *
 *     Scan converts a polygon by returning a run-length
 *     encoding of the resultant bitmap -- the run-length
 *     encoding is in the form of an array of rectangles.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     EdgeTableEntry* pAET;   /* Active Edge Table       */
     int y;                  /* current scanline        */
     int iPts = 0;           /* number of pts in buffer */
     EdgeTableEntry* pWETE;  /* Winding Edge Table Entry*/
     ScanLineList*   pSLL;   /* current scanLineList    */
     ZPointRecord*      pts;    /* output buffer           */
    EdgeTableEntry* pPrevAET;        /* ptr to previous AET     */
    EdgeTable ET;                    /* header node for ET      */
    EdgeTableEntry AET;              /* header node for AET     */
    ScanLineListBlock SLLBlock;      /* header for scanlinelist */
    int         fixWAET = 0;
    POINTBLOCK* curPtBlock;
    int         numFullPtBlocks = 0;
/*.........................................................................*/
    if(a_proc==NULL) return;
    if(Count==0)  return;

    /* special case a rectangle */
    pts = Pts;
    if (((Count == 4) ||
	 ((Count == 5) && (pts[4].x == pts[0].x) && (pts[4].y == pts[0].y))) &&
	(((pts[0].y == pts[1].y) &&
	  (pts[1].x == pts[2].x) &&
	  (pts[2].y == pts[3].y) &&
	  (pts[3].x == pts[0].x)) ||
	 ((pts[0].x == pts[1].x) &&
	  (pts[1].y == pts[2].y) &&
	  (pts[2].x == pts[3].x) &&
	  (pts[3].y == pts[0].y)))) 
      {
        int  xmin,xmax,ymin,ymax;
	xmin = (int)MINIMUM(pts[0].x, pts[2].x);
	ymin = (int)MINIMUM(pts[0].y, pts[2].y);
	xmax = (int)MAXIMUM(pts[0].x, pts[2].x);
	ymax = (int)MAXIMUM(pts[0].y, pts[2].y);
	if ((xmin != xmax) && (ymin != ymax)) 
	    {
              for(y=ymin;y<=ymax;y++)  a_proc(xmin  ,xmax  ,y);
	    }
	return;
    }

    if(Count>Class.pETEn)
      {
	CMemoryFreeBlock          (Class.pETEs);
	Class.pETEn       = Count;
	Class.pETEs       = (EdgeTableEntry*)CMemoryAllocateBlock(Class.pETEn * sizeof(EdgeTableEntry));
	if(Class.pETEs==NULL)  
	  {
	    Class.pETEn = 0;
	    return;
	  }
      }

    CreateETandAET (Count, Pts, &ET, &AET, Class.pETEs, &SLLBlock);

    pSLL           = ET.scanlines.next;

    curPtBlock     = &Class.FirstPtBlock;
    pts            =  Class.FirstPtBlock.pts;
     

    if (rule==0) 
      {
        /*
         *  for each scanline
         */
        for (y = ET.ymin; y < ET.ymax; y++) {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL != NULL && y == pSLL->scanline) 
	      {
                LoadAET(&AET, pSLL->edgelist);
                pSLL = pSLL->next;
	      }
            pPrevAET = &AET;
            pAET = AET.next;
 
            /*
             *  for each active edge
             */
            while (pAET) {
                pts->x = pAET->bres.minor_axis;
                pts->y = y;
                pts++;
                iPts++;
 
                /*
                 *  send out the buffer
                 */
                if (iPts == NUMPTSTOBUFFER) 
		  { 
                    if(numFullPtBlocks < Class.numAllocPtBlocks)
                      {
                        curPtBlock = curPtBlock->next;
                      }
		    else
		      {
			POINTBLOCK*      tmpPtBlock;
			tmpPtBlock       = (POINTBLOCK*)CMemoryAllocateBlock(sizeof(POINTBLOCK));
                        if(tmpPtBlock==NULL)  
			  {
			    FreeStorage(SLLBlock.next);
			    return;
			  }
			tmpPtBlock->next = NULL; /*Barrand*/
			curPtBlock->next = tmpPtBlock;
			curPtBlock       = tmpPtBlock;
			Class.numAllocPtBlocks++;
		      }
		    numFullPtBlocks++;
		    pts  = curPtBlock->pts;
                    iPts = 0;
		  }

                EVALUATEEDGEEVENODD(pAET, pPrevAET, y)
            }
            (void) InsertAndSort(&AET);
        }
      }
    else 
      {
        /*
         *  for each scanline
         */
        for (y = ET.ymin; y < ET.ymax; y++) {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL != NULL && y == pSLL->scanline) 
	      {
                LoadAET(&AET, pSLL->edgelist);
                ComputeWAET(&AET);
                pSLL = pSLL->next;
	      }
            pPrevAET = &AET;
            pAET = AET.next;
            pWETE = pAET;
 
            /*
             *  for each active edge
             */
            while (pAET) {
                /*
                 *  add to the buffer only those edges that
                 *  are in the Winding active edge table.
                 */
                if (pWETE == pAET) {
                    pts->x = pAET->bres.minor_axis;
                    pts->y = y;
                    pts++;
		    iPts++;
 
                    /*
                     *  send out the buffer
                     */
		    if (iPts == NUMPTSTOBUFFER) 
		      { 
			if(numFullPtBlocks < Class.numAllocPtBlocks)
			  {
			    curPtBlock = curPtBlock->next;
			  }
			else
			  {
			    POINTBLOCK*      tmpPtBlock;
			    tmpPtBlock       = (POINTBLOCK *)CMemoryAllocateBlock(sizeof(POINTBLOCK));
                            if(tmpPtBlock==NULL)  
			      {
				FreeStorage(SLLBlock.next);
				return;
			      }
			    tmpPtBlock->next = NULL; /*Barrand*/
			    curPtBlock->next = tmpPtBlock;
			    curPtBlock       = tmpPtBlock;
			    Class.numAllocPtBlocks++;
			  }
			numFullPtBlocks++;
			pts  = curPtBlock->pts;
			iPts = 0;
		      }
                    pWETE = pWETE->nextWETE;
                }
                EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET)
            }
 
            /*
             *  recompute the winding active edge table if
             *  we just resorted or have exited an edge.
             */
            if ( (InsertAndSort(&AET)!=0) || (fixWAET!=0) ) 
	      {
                ComputeWAET(&AET);
                fixWAET = 0;
	      }
        }
      }
    FreeStorage   (SLLBlock.next);	

    ScanPoints (numFullPtBlocks, iPts, &Class.FirstPtBlock,a_proc);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void InsertEdgeInET (
 EdgeTable* ET
,EdgeTableEntry* ETE
,int scanline
,ScanLineListBlock** SLLBlock
,int* iSLLBlock
)
/***************************************************************************/
/*
 *     InsertEdgeInET
 *
 *     Insert the given edge into the edge table.
 *     First we must find the correct bucket in the
 *     Edge table, then find the right slot in the
 *     bucket.  Finally, we can insert it.
 *
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     EdgeTableEntry *start, *prev;
     ScanLineList *pSLL, *pPrevSLL;
    ScanLineListBlock *tmpSLLBlock;
/*.........................................................................*/

    /*
     * find the right bucket to put the edge into
     */
    pPrevSLL = &ET->scanlines;
    pSLL = pPrevSLL->next;
    while (pSLL && (pSLL->scanline < scanline)) 
    {
        pPrevSLL = pSLL;
        pSLL = pSLL->next;
    }

    /*
     * reassign pSLL (pointer to ScanLineList) if necessary
     */
    if ( (pSLL==NULL) || (pSLL->scanline > scanline)) 
    {
        if (*iSLLBlock > SLLSPERBLOCK-1) 
        {
            tmpSLLBlock = (ScanLineListBlock *)CMemoryAllocateBlock(sizeof(ScanLineListBlock));
            (*SLLBlock)->next = tmpSLLBlock;
            tmpSLLBlock->next = (ScanLineListBlock *)NULL;
            *SLLBlock = tmpSLLBlock;
            *iSLLBlock = 0;
        }
        pSLL = &((*SLLBlock)->SLLs[(*iSLLBlock)++]);

        pSLL->next = pPrevSLL->next;
        pSLL->edgelist = (EdgeTableEntry *)NULL;
        pPrevSLL->next = pSLL;
    }
    pSLL->scanline = scanline;

    /*
     * now insert the edge in the right bucket
     */
    prev = (EdgeTableEntry *)NULL;
    start = pSLL->edgelist;
    while (start && (start->bres.minor_axis < ETE->bres.minor_axis)) 
    {
        prev = start;
        start = start->next;
    }
    ETE->next = start;

    if (prev!=NULL)
        prev->next = ETE;
    else
        pSLL->edgelist = ETE;
}

/***************************************************************************/
static void CreateETandAET (
  int count
, ZPointRecord* pts
,EdgeTable* ET
,EdgeTableEntry* AET
, EdgeTableEntry* pETEs
,ScanLineListBlock* pSLLBlock
)
/***************************************************************************/
/*
 *     CreateEdgeTable
 *
 *     This routine creates the edge table for
 *     scan converting polygons. 
 *     The Edge Table (ET) looks like:
 *
 *    EdgeTable
 *     --------
 *    |  ymax  |        ScanLineLists
 *    |scanline|-->------------>-------------->...
 *     --------   |scanline|   |scanline|
 *                |edgelist|   |edgelist|
 *                ---------    ---------
 *                    |             |
 *                    |             |
 *                    V             V
 *              list of ETEs   list of ETEs
 *
 *     where ETE is an EdgeTableEntry data structure,
 *     and there is one ScanLineList per scanline at
 *     which an edge is initially entered.
 *
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     ZPointRecord *top, *bottom;
     ZPointRecord *PrevPt, *CurrPt;
    int iSLLBlock = 0;
    int dy;
/*.........................................................................*/

    if (count < 2)  return;

    /*
     *  initialize the Active Edge Table
     */
    AET->next = (EdgeTableEntry *)NULL;
    AET->back = (EdgeTableEntry *)NULL;
    AET->nextWETE = (EdgeTableEntry *)NULL;
    AET->bres.minor_axis = SMALL_COORDINATE;

    /*
     *  initialize the Edge Table.
     */
    ET->scanlines.next = (ScanLineList *)NULL;
    ET->ymax = SMALL_COORDINATE;
    ET->ymin = LARGE_COORDINATE;
    pSLLBlock->next = (ScanLineListBlock *)NULL;

    PrevPt = &pts[count-1];

    /*
     *  for each vertex in the array of points.
     *  In this loop we are dealing with two vertices at
     *  a time -- these make up one edge of the polygon.
     */
    while (count--) 
    {
        CurrPt = pts++;

        /*
         *  find out which point is above and which is below.
         */
        if (PrevPt->y > CurrPt->y) 
        {
            bottom = PrevPt, top = CurrPt;
            pETEs->ClockWise = 0;
        }
        else 
        {
            bottom = CurrPt, top = PrevPt;
            pETEs->ClockWise = 1;
        }

        /*
         * don't add horizontal edges to the Edge table.
         */
        if (bottom->y != top->y) 
        {
            pETEs->ymax = (int)(bottom->y-1);  /* -1 so we don't get last scanline */

            /*
             *  initialize integer edge algorithm
             */
            dy = (int)(bottom->y - top->y);
            BRESINITPGONSTRUCT (dy,(int)top->x,(int)bottom->x, pETEs->bres)

            InsertEdgeInET (ET, pETEs, (int)top->y, &pSLLBlock, &iSLLBlock);

	    if (PrevPt->y > ET->ymax) ET->ymax = (int) PrevPt->y;
	    if (PrevPt->y < ET->ymin) ET->ymin = (int) PrevPt->y;
            pETEs++;
        }

        PrevPt = CurrPt;
    }
}
/***************************************************************************/
static void LoadAET (
  EdgeTableEntry* AET
, EdgeTableEntry* ETEs
)
/***************************************************************************/
/*
 *     LoadAET
 *
 *     This routine moves EdgeTableEntries from the
 *     EdgeTable into the Active Edge Table,
 *     leaving them sorted by smaller x coordinate.
 *
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     EdgeTableEntry *pPrevAET;
     EdgeTableEntry *tmp;
/*.........................................................................*/

    pPrevAET = AET;
    AET = AET->next;
    while (ETEs) 
    {
        while (AET && (AET->bres.minor_axis < ETEs->bres.minor_axis)) 
        {
            pPrevAET = AET;
            AET = AET->next;
        }
        tmp = ETEs->next;
        ETEs->next = AET;
        if (AET!=NULL)
            AET->back = ETEs;
        ETEs->back = pPrevAET;
        pPrevAET->next = ETEs;
        pPrevAET = ETEs;

        ETEs = tmp;
    }
}
/***************************************************************************/
static void ComputeWAET (
  EdgeTableEntry* AET
)
/***************************************************************************/
/*
 *     ComputeWAET
 *
 *     This routine links the AET by the
 *     nextWETE (winding EdgeTableEntry) link for
 *     use by the winding number rule.  The final 
 *     Active Edge Table (AET) might look something
 *     like:
 *
 *     AET
 *     ----------  ---------   ---------
 *     |ymax    |  |ymax    |  |ymax    | 
 *     | ...    |  |...     |  |...     |
 *     |next    |->|next    |->|next    |->...
 *     |nextWETE|  |nextWETE|  |nextWETE|
 *     ---------   ---------   ^--------
 *         |                   |       |
 *         V------------------->       V---> ...
 *
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     EdgeTableEntry *pWETE;
     int inside = 1;
     int isInside = 0;
/*.........................................................................*/
    AET->nextWETE = (EdgeTableEntry *)NULL;
    pWETE = AET;
    AET = AET->next;
    while (AET) 
    {
        if (AET->ClockWise!=0)
            isInside++;
        else
            isInside--;

        if (( (inside==0) && (isInside==0) ) ||
            ( (inside!=0) && (isInside!=0) )) 
        {
            pWETE->nextWETE = AET;
            pWETE = AET;
            inside = !inside;
        }
        AET = AET->next;
    }
    pWETE->nextWETE = (EdgeTableEntry *)NULL;
}
/***************************************************************************/
static int InsertAndSort (
  EdgeTableEntry* AET
)
/***************************************************************************/
/*
 *     InsertAndSort
 *
 *     Just a simple insertion sort using
 *     pointers and back pointers to sort the Active
 *     Edge Table.
 *
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     EdgeTableEntry *pETEchase;
     EdgeTableEntry *pETEinsert;
     EdgeTableEntry *pETEchaseBackTMP;
     int changed = 0;
/*.........................................................................*/

    AET = AET->next;
    while (AET) 
    {
        pETEinsert = AET;
        pETEchase = AET;
        while (pETEchase->back->bres.minor_axis > AET->bres.minor_axis)
            pETEchase = pETEchase->back;

        AET = AET->next;
        if (pETEchase != pETEinsert) 
        {
            pETEchaseBackTMP = pETEchase->back;
            pETEinsert->back->next = AET;
            if (AET!=NULL)
                AET->back = pETEinsert->back;
            pETEinsert->next = pETEchase;
            pETEchase->back->next = pETEinsert;
            pETEchase->back = pETEinsert;
            pETEinsert->back = pETEchaseBackTMP;
            changed = 1;
        }
    }
    return(changed);
}
/***************************************************************************/
static void FreeStorage (
  ScanLineListBlock* pSLLBlock
)
/***************************************************************************/
/*
 *     Clean up our act.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
     ScanLineListBlock   *tmpSLLBlock;
/*.........................................................................*/
    while (pSLLBlock) 
    {
        tmpSLLBlock = pSLLBlock->next;
        CMemoryFreeBlock    (pSLLBlock);
        pSLLBlock   = tmpSLLBlock;
    }
}
/***************************************************************************/
static void ScanPoints (
  int  numFullPtBlocks
, int  iCurPtBlock
,POINTBLOCK* FirstPtBlock
,ZPolygonScanFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   ZPointRecord*  pts;
   POINTBLOCK* CurPtBlock;
   int         i;
/*.........................................................................*/
  CurPtBlock = FirstPtBlock;
  for ( ; numFullPtBlocks >= 0; numFullPtBlocks--) 
    {
      /* the loop uses 2 points per iteration */
      i = numFullPtBlocks!=0 ? NUMPTSTOBUFFER >> 1 : iCurPtBlock >> 1 ;
      for (pts = CurPtBlock->pts; i--; pts += 2) 
	{
	  a_proc ((int)(pts->x),(int)pts[1].x,(int)pts->y);
	}
      CurPtBlock = CurPtBlock->next;
    }
}
