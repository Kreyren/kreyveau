 /***************************************************************************\
|*                                                                           *|
|*       Copyright 2003 NVIDIA, Corporation.  All rights reserved.           *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 2003 NVIDIA, Corporation.  All rights reserved.           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/nv/nv_xaa.c,v 1.35 2004/03/20 16:25:18 mvojkovi Exp $ */

#include "nv_include.h"
#include "xaalocal.h"
#include "miline.h"
#include "nv_dma.h"
#include "nvreg.h"

static const int NVCopyROP[16] =
{
   0x00,            /* GXclear */
   0x88,            /* GXand */
   0x44,            /* GXandReverse */
   0xCC,            /* GXcopy */
   0x22,            /* GXandInverted */
   0xAA,            /* GXnoop */
   0x66,            /* GXxor */
   0xEE,            /* GXor */
   0x11,            /* GXnor */
   0x99,            /* GXequiv */
   0x55,            /* GXinvert*/
   0xDD,            /* GXorReverse */
   0x33,            /* GXcopyInverted */
   0xBB,            /* GXorInverted */
   0x77,            /* GXnand */
   0xFF             /* GXset */
};

static const int NVCopyROP_PM[16] =
{
   0x0A,            /* GXclear */
   0x8A,            /* GXand */
   0x4A,            /* GXandReverse */
   0xCA,            /* GXcopy */
   0x2A,            /* GXandInverted */
   0xAA,            /* GXnoop */
   0x6A,            /* GXxor */
   0xEA,            /* GXor */
   0x1A,            /* GXnor */
   0x9A,            /* GXequiv */
   0x5A,            /* GXinvert*/
   0xDA,            /* GXorReverse */
   0x3A,            /* GXcopyInverted */
   0xBA,            /* GXorInverted */
   0x7A,            /* GXnand */
   0xFA             /* GXset */
};

static const int NVPatternROP[16] =
{
   0x00,
   0xA0,
   0x50,
   0xF0,
   0x0A,
   0xAA,
   0x5A,
   0xFA,
   0x05,
   0xA5,
   0x55,
   0xF5,
   0x0F,
   0xAF,
   0x5F,
   0xFF
};

void
NVDmaKickoff(NVPtr pNv)
{
    if(pNv->dmaCurrent != pNv->dmaPut) {
        pNv->dmaPut = pNv->dmaCurrent;
        WRITE_PUT(pNv,  pNv->dmaPut);
    }
}

static CARD32 getOffset(NVPtr pNv, NVAllocRec *mem) {
	return (mem->offset - pNv->VRAMPhysical);
}

/* There is a HW race condition with videoram command buffers.
   You can't jump to the location of your put offset.  We write put
   at the jump offset + SKIPS dwords with noop padding in between
   to solve this problem */
#define SKIPS  8

#if NV_DMA_DEBUG == 1
CARD32 READ_GET(void *data) {
	NVPtr pNv = data;

	CARD32 getPtr = pNv->FIFO[0x44/4];
	xf86DrvMsg(0, X_INFO, "READ_GET: 0x%08x\n", getPtr);
	getPtr = (getPtr - pNv->fifo.put_base) >> 2;
	xf86DrvMsg(0, X_INFO, "READ_GET retval: 0x%08x\n", getPtr);
	return getPtr;
}
#endif

void 
NVDmaWait (NVPtr pNv, int size){
    int t_start;
    int dmaGet;

    size++;

    t_start = GetTimeInMillis();
    while(pNv->dmaFree < size) {
       dmaGet = READ_GET(pNv);

       if(pNv->dmaPut >= dmaGet) {
           pNv->dmaFree = pNv->dmaMax - pNv->dmaCurrent;
           if(pNv->dmaFree < size) {
               NVDmaNext(pNv, (0x20000000|pNv->fifo.put_base));
               if(dmaGet <= SKIPS) {
                   if(pNv->dmaPut <= SKIPS) /* corner case - will be idle */
                      WRITE_PUT(pNv, SKIPS + 1);
                   do {
                       if (GetTimeInMillis() - t_start > 2000)
                           NVDoSync(pNv);
                       dmaGet = READ_GET(pNv);
                   } while(dmaGet <= SKIPS);
               }
               WRITE_PUT(pNv, SKIPS);
               pNv->dmaCurrent = pNv->dmaPut = SKIPS;
               pNv->dmaFree = dmaGet - (SKIPS + 1);
           }
       } else 
           pNv->dmaFree = dmaGet - pNv->dmaCurrent - 1;

       if (GetTimeInMillis() - t_start > 2000)
           NVDoSync(pNv);
    }
}

void
NVWaitVSync(NVPtr pNv)
{
    NVDmaStart(pNv, 5, 0x0000012C, 1);
    NVDmaNext (pNv, 0);
    NVDmaStart(pNv, 5, 0x00000134, 1);
    NVDmaNext (pNv, pNv->CRTCnumber);
    NVDmaStart(pNv, 5, 0x00000100, 1);
    NVDmaNext (pNv, 0);
    NVDmaStart(pNv, 5, 0x00000130, 1);
    NVDmaNext (pNv, 0);
}

/* 
  currentRop =  0-15  solid fill
               16-31  8x8 pattern fill
               32-47  solid fill with planemask 
*/

static void 
NVSetPattern(
   ScrnInfoPtr pScrn,
   CARD32 clr0,
   CARD32 clr1,
   CARD32 pat0,
   CARD32 pat1
)
{
    NVPtr pNv = NVPTR(pScrn);

    NVDmaStart(pNv, NvSubImagePattern, PATTERN_COLOR_0, 4);
    NVDmaNext (pNv, clr0);
    NVDmaNext (pNv, clr1);
    NVDmaNext (pNv, pat0);
    NVDmaNext (pNv, pat1);
}

void 
NVSetRopSolid(ScrnInfoPtr pScrn, CARD32 rop, CARD32 planemask)
{
    NVPtr pNv = NVPTR(pScrn);

    if(planemask != ~0) {
        NVSetPattern(pScrn, 0, planemask, ~0, ~0);
        if(pNv->currentRop != (rop + 32)) {
           NVDmaStart(pNv, NvSubRop, ROP_SET, 1);
           NVDmaNext (pNv, NVCopyROP_PM[rop]);
           pNv->currentRop = rop + 32;
        }
    } else 
    if (pNv->currentRop != rop) {
        if(pNv->currentRop >= 16)
             NVSetPattern(pScrn, ~0, ~0, ~0, ~0);
        NVDmaStart(pNv, NvSubRop, ROP_SET, 1);
        NVDmaNext (pNv, NVCopyROP[rop]);
        pNv->currentRop = rop;
    }
}

void NVResetGraphics(ScrnInfoPtr pScrn)
{
    NVPtr pNv = NVPTR(pScrn);
    CARD32 surfaceFormat, patternFormat, rectFormat, lineFormat;
    int pitch, i;

    if(pNv->NoAccel) return;

    pitch = pNv->CurrentLayout.displayWidth * 
            (pNv->CurrentLayout.bitsPerPixel >> 3);

    pNv->dmaPut = pNv->dmaCurrent = 0;
    pNv->dmaMax = pNv->dmaFree = (pNv->fifo.cmdbuf_size >> 2) - 1;

    for (i=0; i<SKIPS; i++)
        NVDmaNext(pNv, 0);
    pNv->dmaFree -= SKIPS;

    NVDmaSetObjectOnSubchannel(pNv, NvSubContextSurfaces, NvContextSurfaces);
    NVDmaSetObjectOnSubchannel(pNv, NvSubRop            , NvRop            );
    NVDmaSetObjectOnSubchannel(pNv, NvSubImagePattern   , NvImagePattern   );
    NVDmaSetObjectOnSubchannel(pNv, NvSubClipRectangle  , NvClipRectangle  );
    NVDmaSetObjectOnSubchannel(pNv, NvSubSolidLine      , NvSolidLine      );
    NVDmaSetObjectOnSubchannel(pNv, NvSubImageBlit      , NvImageBlit      );
    NVDmaSetObjectOnSubchannel(pNv, NvSubRectangle      , NvRectangle      );
    NVDmaSetObjectOnSubchannel(pNv, NvSubScaledImage    , NvScaledImage    );

    switch(pNv->CurrentLayout.depth) {
    case 24:
       surfaceFormat = SURFACE_FORMAT_X8R8G8B8;
       patternFormat = PATTERN_FORMAT_DEPTH24;
       rectFormat    = RECT_FORMAT_DEPTH24;
       lineFormat    = LINE_FORMAT_DEPTH24;
       break;
    case 16:
    case 15:
       surfaceFormat = SURFACE_FORMAT_R5G6B5;
       patternFormat = PATTERN_FORMAT_DEPTH16;
       rectFormat    = RECT_FORMAT_DEPTH16;
       lineFormat    = LINE_FORMAT_DEPTH16;
       break;
    default:
       surfaceFormat = SURFACE_FORMAT_Y8;
       patternFormat = PATTERN_FORMAT_DEPTH8;
       rectFormat    = RECT_FORMAT_DEPTH8;
       lineFormat    = LINE_FORMAT_DEPTH8;
       break;
    }

    NVDmaStart(pNv, NvSubContextSurfaces, SURFACE_FORMAT, 4);
    NVDmaNext (pNv, surfaceFormat);
    NVDmaNext (pNv, pitch | (pitch << 16));
    NVDmaNext (pNv, getOffset(pNv, pNv->FB));
    NVDmaNext (pNv, getOffset(pNv, pNv->FB));

    NVDmaStart(pNv, NvSubImagePattern, PATTERN_FORMAT, 1);
    NVDmaNext (pNv, patternFormat);

    NVDmaStart(pNv, NvSubRectangle, RECT_FORMAT, 1);
    NVDmaNext (pNv, rectFormat);

    NVDmaStart(pNv, NvSubSolidLine, LINE_FORMAT, 1);
    NVDmaNext (pNv, lineFormat);

    pNv->currentRop = ~0;  /* set to something invalid */
    NVSetRopSolid(pScrn, GXcopy, ~0);

    /*NVDmaKickoff(pNv);*/
}

static void NVDumpLockupInfo(NVPtr pNv)
{
	int i,start;
	start=READ_GET(pNv)-10;
	if (start<0) start=0;
	xf86DrvMsg(0, X_INFO, "Fifo dump (lockup 0x%04x,0x%04x):\n",READ_GET(pNv),pNv->dmaPut);
	for(i=start;i<pNv->dmaPut+10;i++)
		xf86DrvMsg(0, X_INFO, "[0x%04x] 0x%08x\n", i, pNv->dmaBase[i]);
	xf86DrvMsg(0, X_INFO, "End of fifo dump\n");
}

void NVDoSync(NVPtr pNv)
{
    int t_start, timeout = 2000;

    if(pNv->DMAKickoffCallback)
       (*pNv->DMAKickoffCallback)(pNv);

    t_start = GetTimeInMillis();
    while((GetTimeInMillis() - t_start) < timeout && (READ_GET(pNv) != pNv->dmaPut));
    while((GetTimeInMillis() - t_start) < timeout && pNv->PGRAPH[NV_PGRAPH_STATUS/4]);

    if ((GetTimeInMillis() - t_start) >= timeout) {
        if (pNv->LockedUp)
            return;
        NVDumpLockupInfo(pNv);
        pNv->LockedUp = TRUE; /* avoid re-entering FatalError on shutdown */
        FatalError("DMA queue hang: dmaPut=%x, current=%x, status=%x\n",
               pNv->dmaPut, READ_GET(pNv), pNv->PGRAPH[NV_PGRAPH_STATUS/4]);
    }
}

void NVSync(ScrnInfoPtr pScrn)
{
    NVPtr pNv = NVPTR(pScrn);
    NVDoSync(pNv);
}

void
NVDMAKickoffCallback (NVPtr pNv)
{
   NVDmaKickoff(pNv);
   pNv->DMAKickoffCallback = NULL;
}


static void
NVSetupForScreenToScreenCopy(
   ScrnInfoPtr pScrn, 
   int xdir, int ydir, 
   int rop,
   unsigned planemask, 
   int transparency_color
)
{
    NVPtr pNv = NVPTR(pScrn);

    planemask |= ~0 << pNv->CurrentLayout.depth;

    NVSetRopSolid(pScrn, rop, planemask);

    pNv->DMAKickoffCallback = NVDMAKickoffCallback;
}

static void
NVSubsequentScreenToScreenCopy(
   ScrnInfoPtr pScrn, 
   int x1, int y1,
   int x2, int y2, 
   int w, int h
)
{
    NVPtr pNv = NVPTR(pScrn);

    NVDmaStart(pNv, NvSubImageBlit, BLIT_POINT_SRC, 3);
    NVDmaNext (pNv, (y1 << 16) | x1);
    NVDmaNext (pNv, (y2 << 16) | x2);
    NVDmaNext (pNv, (h  << 16) | w);

    if((w * h) >= 512)
       NVDmaKickoff(pNv); 
}

static void
NVSetupForSolidFill(
   ScrnInfoPtr pScrn, 
   int color, 
   int rop,
   unsigned planemask
)
{
   NVPtr pNv = NVPTR(pScrn);

   planemask |= ~0 << pNv->CurrentLayout.depth;

   NVSetRopSolid(pScrn, rop, planemask);
   NVDmaStart(pNv, NvSubRectangle, RECT_SOLID_COLOR, 1);
   NVDmaNext (pNv, color);

   pNv->DMAKickoffCallback = NVDMAKickoffCallback;
}

static void
NVSubsequentSolidFillRect(ScrnInfoPtr pScrn, int x, int y, int w, int h)
{
   NVPtr pNv = NVPTR(pScrn);

   NVDmaStart(pNv, NvSubRectangle, RECT_SOLID_RECTS(0), 2);
   NVDmaNext (pNv, (x << 16) | y);
   NVDmaNext (pNv, (w << 16) | h);

   if((w * h) >= 512)
      NVDmaKickoff(pNv);
}

static void
NVSetupForMono8x8PatternFill (
   ScrnInfoPtr pScrn, 
   int patternx, int patterny,
   int fg, int bg, 
   int rop, 
   unsigned planemask
)
{
   NVPtr pNv = NVPTR(pScrn);

   planemask = ~0 << pNv->CurrentLayout.depth;

   fg |= planemask;
   if(bg == -1) bg = 0;
   else bg |= planemask;

   if (pNv->currentRop != (rop + 16)) {
       NVDmaStart(pNv, NvSubRop, ROP_SET, 1);
       NVDmaNext (pNv, NVPatternROP[rop]);
       pNv->currentRop = rop + 16;
   }

   NVSetPattern(pScrn, bg, fg, patternx, patterny);
   NVDmaStart(pNv, NvSubRectangle, RECT_SOLID_COLOR, 1);
   NVDmaNext (pNv, fg);

   pNv->DMAKickoffCallback = NVDMAKickoffCallback;
}

static void
NVSubsequentMono8x8PatternFillRect(
   ScrnInfoPtr pScrn,
   int patternx, int patterny,
   int x, int y, 
   int w, int h
)
{
   NVPtr pNv = NVPTR(pScrn);

   NVDmaStart(pNv, NvSubRectangle, RECT_SOLID_RECTS(0), 2);
   NVDmaNext (pNv, (x << 16) | y);
   NVDmaNext (pNv, (w << 16) | h);

   if((w * h) >= 512)
      NVDmaKickoff(pNv);
}

static CARD32 _bg_pixel;
static CARD32 _fg_pixel;
static Bool _transparent;
static CARD32 _color_expand_dwords;
static CARD32 _color_expand_offset;
static int _remaining;
static unsigned char *_storage_buffer[1];

static void
NVSetupForScanlineCPUToScreenColorExpandFill (
   ScrnInfoPtr pScrn,
   int fg, int bg,
   int rop,
   unsigned int planemask
)
{
   NVPtr pNv = NVPTR(pScrn);

   CARD32 mask = ~0 << pNv->CurrentLayout.depth;

   planemask |= mask;
   _fg_pixel = fg | mask;

   if(bg == -1) {
      _transparent = TRUE;
   } else {
      _transparent = FALSE;
      _bg_pixel = bg | mask;
   }

   NVSetRopSolid (pScrn, rop, planemask);
}

static void
NVSubsequentScanlineCPUToScreenColorExpandFill (
    ScrnInfoPtr pScrn, 
    int x, int y,
    int w, int h,
    int skipleft
)
{
   NVPtr pNv = NVPTR(pScrn);
   int bw = (w + 31) & ~31;

   _color_expand_dwords = bw >> 5;
   _remaining = h;

   if(_transparent) {
      NVDmaStart(pNv, NvSubRectangle, RECT_EXPAND_ONE_COLOR_CLIP, 5);
      NVDmaNext (pNv, (y << 16) | ((x + skipleft) & 0xFFFF));
      NVDmaNext (pNv, ((y + h) << 16) | ((x + w) & 0xFFFF));
      NVDmaNext (pNv, _fg_pixel);
      NVDmaNext (pNv, (h << 16) | bw);
      NVDmaNext (pNv, (y << 16) | (x & 0xFFFF));
      _color_expand_offset = RECT_EXPAND_ONE_COLOR_DATA(0);
   } else {
      NVDmaStart(pNv, NvSubRectangle, RECT_EXPAND_TWO_COLOR_CLIP, 7);
      NVDmaNext (pNv, (y << 16) | ((x + skipleft) & 0xFFFF));
      NVDmaNext (pNv, ((y + h) << 16) | ((x + w) & 0xFFFF));
      NVDmaNext (pNv, _bg_pixel);
      NVDmaNext (pNv, _fg_pixel);
      NVDmaNext (pNv, (h << 16) | bw);
      NVDmaNext (pNv, (h << 16) | bw);
      NVDmaNext (pNv, (y << 16) | (x & 0xFFFF));
      _color_expand_offset = RECT_EXPAND_TWO_COLOR_DATA(0); 
   }

   NVDmaStart(pNv, NvSubRectangle, _color_expand_offset, _color_expand_dwords);
   _storage_buffer[0] = (unsigned char*)&pNv->dmaBase[pNv->dmaCurrent];
}

static void
NVSubsequentColorExpandScanline(ScrnInfoPtr pScrn, int bufno)
{
   NVPtr pNv = NVPTR(pScrn);

   pNv->dmaCurrent += _color_expand_dwords;

   if(--_remaining) {
       NVDmaStart(pNv, NvSubRectangle, _color_expand_offset, _color_expand_dwords);
       _storage_buffer[0] = (unsigned char*)&pNv->dmaBase[pNv->dmaCurrent];
   } else {
       /* hardware bug workaround */
       NVDmaStart(pNv, NvSubImageBlit, BLIT_POINT_SRC, 1);
       NVDmaNext (pNv, 0);
       NVDmaKickoff(pNv);
   }
}

static void 
NVSetupForScanlineImageWrite(
   ScrnInfoPtr pScrn, int rop, 
   unsigned int planemask, 
   int trans_color, 
   int bpp, int depth
)
{
   NVPtr pNv = NVPTR(pScrn);

   planemask |= ~0 << pNv->CurrentLayout.depth;

   NVSetRopSolid (pScrn, rop, planemask);
}

static CARD32 _image_size;
static CARD32 _image_srcpoint;
static CARD32 _image_dstpoint;
static CARD32 _image_dstpitch;

static void 
NVSubsequentScanlineImageWriteRect(
   ScrnInfoPtr pScrn, 
   int x, int y, 
   int w, int h, 
   int skipleft
)
{
   NVPtr pNv = NVPTR(pScrn);
   int Bpp = pNv->CurrentLayout.bitsPerPixel >> 3;
   int image_srcpitch;

   _image_size = (1 << 16) | (w - skipleft);
   _image_srcpoint = skipleft;
   _image_dstpoint = (y << 16) | (x + skipleft);
   _remaining = h;
   _image_dstpitch = pNv->CurrentLayout.displayWidth * Bpp;
   image_srcpitch =  ((w * Bpp) + 63) & ~63;
   _storage_buffer[0] = pNv->ScratchBuffer->map;

   NVSync(pScrn);

   NVDmaStart(pNv, NvSubContextSurfaces, SURFACE_PITCH, 2);
   NVDmaNext (pNv, (_image_dstpitch << 16) | image_srcpitch);
   NVDmaNext (pNv, getOffset(pNv, pNv->ScratchBuffer));
}

static void NVSubsequentImageWriteScanline(ScrnInfoPtr pScrn, int bufno)
{
   NVPtr pNv = NVPTR(pScrn);

   NVDmaStart(pNv, NvSubImageBlit, BLIT_POINT_SRC, 3);
   NVDmaNext (pNv, _image_srcpoint);
   NVDmaNext (pNv, _image_dstpoint);
   NVDmaNext (pNv, _image_size);
   NVDmaKickoff(pNv);

   if(--_remaining) {
      _image_dstpoint += (1 << 16);
      NVSync(pScrn);
   } else {
      NVDmaStart(pNv, NvSubContextSurfaces, SURFACE_PITCH, 2);
      NVDmaNext (pNv, _image_dstpitch | (_image_dstpitch << 16));
      NVDmaNext (pNv, getOffset(pNv, pNv->FB));
   }
}

static void
NVSetupForSolidLine(ScrnInfoPtr pScrn, int color, int rop, unsigned planemask)
{
    NVPtr pNv = NVPTR(pScrn);

    planemask |= ~0 << pNv->CurrentLayout.depth;

    NVSetRopSolid(pScrn, rop, planemask);

    _fg_pixel = color;

    pNv->DMAKickoffCallback = NVDMAKickoffCallback;
}

static void 
NVSubsequentSolidHorVertLine(ScrnInfoPtr pScrn, int x, int y, int len, int dir)
{
    NVPtr pNv = NVPTR(pScrn);

    NVDmaStart(pNv, NvSubSolidLine, LINE_COLOR, 1);
    NVDmaNext (pNv, _fg_pixel);
    NVDmaStart(pNv, NvSubSolidLine, LINE_LINES(0), 2);
    NVDmaNext (pNv, (y << 16) | ( x & 0xffff));
    if(dir == DEGREES_0) {
       NVDmaNext (pNv, (y << 16) | ((x + len) & 0xffff));
    } else {
       NVDmaNext (pNv, ((y + len) << 16) | (x & 0xffff));
    }
}

static void 
NVSubsequentSolidTwoPointLine(
   ScrnInfoPtr pScrn, 
   int x1, int y1,
   int x2, int y2, 
   int flags
)
{
    NVPtr pNv = NVPTR(pScrn);
    Bool drawLast = !(flags & OMIT_LAST);

    NVDmaStart(pNv, NvSubSolidLine, LINE_COLOR, 1);
    NVDmaNext (pNv, _fg_pixel);
    NVDmaStart(pNv, NvSubSolidLine, LINE_LINES(0), drawLast ? 4 : 2);
    NVDmaNext (pNv, (y1 << 16) | (x1 & 0xffff));
    NVDmaNext (pNv, (y2 << 16) | (x2 & 0xffff));
    if(drawLast) {
        NVDmaNext (pNv, (y2 << 16) | (x2 & 0xffff));
        NVDmaNext (pNv, ((y2 + 1) << 16) | (x2 & 0xffff));
    }
}

static void
NVSetClippingRectangle(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2)
{
    NVPtr pNv = NVPTR(pScrn);
    int h = y2 - y1 + 1;
    int w = x2 - x1 + 1;

    NVDmaStart(pNv, NvSubClipRectangle, CLIP_POINT, 2);
    NVDmaNext (pNv, (y1 << 16) | x1); 
    NVDmaNext (pNv, (h << 16) | w);
}

static void
NVDisableClipping(ScrnInfoPtr pScrn)
{
    NVPtr pNv = NVPTR(pScrn);

    NVDmaStart(pNv, NvSubClipRectangle, CLIP_POINT, 2);
    NVDmaNext (pNv, 0);              
    NVDmaNext (pNv, 0x7FFF7FFF);
}


/* Initialize XAA acceleration info */
Bool
NVXaaInit(ScreenPtr pScreen) 
{
   ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
   NVPtr pNv = NVPTR(pScrn);
   XAAInfoRecPtr accel;

   accel = pNv->AccelInfoRec = XAACreateInfoRec();
   if(!accel) return FALSE;

   accel->Flags = LINEAR_FRAMEBUFFER | PIXMAP_CACHE | OFFSCREEN_PIXMAPS;
   accel->Sync = NVSync;

   accel->ScreenToScreenCopyFlags = NO_TRANSPARENCY;
   accel->SetupForScreenToScreenCopy = NVSetupForScreenToScreenCopy;
   accel->SubsequentScreenToScreenCopy = NVSubsequentScreenToScreenCopy;

   accel->SolidFillFlags = 0;
   accel->SetupForSolidFill = NVSetupForSolidFill;
   accel->SubsequentSolidFillRect = NVSubsequentSolidFillRect;

   accel->Mono8x8PatternFillFlags = HARDWARE_PATTERN_SCREEN_ORIGIN |
                                    HARDWARE_PATTERN_PROGRAMMED_BITS |
                                    NO_PLANEMASK;
   accel->SetupForMono8x8PatternFill = NVSetupForMono8x8PatternFill;
   accel->SubsequentMono8x8PatternFillRect = NVSubsequentMono8x8PatternFillRect;

   accel->ScanlineCPUToScreenColorExpandFillFlags = 
                                    BIT_ORDER_IN_BYTE_LSBFIRST |
                                    CPU_TRANSFER_PAD_DWORD |
                                    LEFT_EDGE_CLIPPING |
                                    LEFT_EDGE_CLIPPING_NEGATIVE_X;
   accel->NumScanlineColorExpandBuffers = 1;
   accel->SetupForScanlineCPUToScreenColorExpandFill = 
            NVSetupForScanlineCPUToScreenColorExpandFill;
   accel->SubsequentScanlineCPUToScreenColorExpandFill = 
            NVSubsequentScanlineCPUToScreenColorExpandFill;
   accel->SubsequentColorExpandScanline = 
            NVSubsequentColorExpandScanline;
   accel->ScanlineColorExpandBuffers = _storage_buffer;

   accel->ScanlineImageWriteFlags = NO_GXCOPY |
                                    NO_TRANSPARENCY |
                                    LEFT_EDGE_CLIPPING |
                                    LEFT_EDGE_CLIPPING_NEGATIVE_X;
   accel->NumScanlineImageWriteBuffers = 1;
   accel->SetupForScanlineImageWrite = NVSetupForScanlineImageWrite;
   accel->SubsequentScanlineImageWriteRect = NVSubsequentScanlineImageWriteRect;
   accel->SubsequentImageWriteScanline = NVSubsequentImageWriteScanline;
   accel->ScanlineImageWriteBuffers = _storage_buffer;

   accel->SolidLineFlags = 0;
   accel->SetupForSolidLine = NVSetupForSolidLine;
   accel->SubsequentSolidHorVertLine = NVSubsequentSolidHorVertLine;
   accel->SubsequentSolidTwoPointLine = NVSubsequentSolidTwoPointLine;
   accel->SetClippingRectangle = NVSetClippingRectangle;
   accel->DisableClipping = NVDisableClipping;
   accel->ClippingFlags = HARDWARE_CLIP_SOLID_LINE;
   
   miSetZeroLineBias(pScreen, OCTANT1 | OCTANT3 | OCTANT4 | OCTANT6);

   return (XAAInit(pScreen, accel));
}

