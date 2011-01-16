#ifndef NV_OBJECT_XML
#define NV_OBJECT_XML

/* Autogenerated file, DO NOT EDIT manually!

This file was generated by the rules-ng-ng headergen tool in this git repository:
http://0x04.net/cgit/index.cgi/rules-ng-ng
git clone git://0x04.net/rules-ng-ng

The rules-ng-ng source files this header was generated from are:
- nv_object.xml  (  11092 bytes, from 2010-12-15 23:45:18)
- copyright.xml  (   6452 bytes, from 2010-12-15 23:45:18)
- nvchipsets.xml (   3074 bytes, from 2010-12-15 23:45:18)
- nv_defs.xml    (   4437 bytes, from 2010-12-15 23:45:18)

Copyright (C) 2006-2010 by the following authors:
- Artur Huillet <arthur.huillet@free.fr> (ahuillet)
- Ben Skeggs (darktama, darktama_)
- B. R. <koala_br@users.sourceforge.net> (koala_br)
- Carlos Martin <carlosmn@users.sf.net> (carlosmn)
- Christoph Bumiller <e0425955@student.tuwien.ac.at> (calim, chrisbmr)
- Dawid Gajownik <gajownik@users.sf.net> (gajownik)
- Dmitry Baryshkov
- Dmitry Eremin-Solenikov <lumag@users.sf.net> (lumag)
- EdB <edb_@users.sf.net> (edb_)
- Erik Waling <erikwailing@users.sf.net> (erikwaling)
- Francisco Jerez <currojerez@riseup.net> (curro)
- imirkin <imirkin@users.sf.net> (imirkin)
- jb17bsome <jb17bsome@bellsouth.net> (jb17bsome)
- Jeremy Kolb <kjeremy@users.sf.net> (kjeremy)
- Laurent Carlier <lordheavym@gmail.com> (lordheavy)
- Luca Barbieri <luca@luca-barbieri.com> (lb, lb1)
- Maarten Maathuis <madman2003@gmail.com> (stillunknown)
- Marcin Kościelnicki <koriakin@0x04.net> (mwk, koriakin)
- Mark Carey <mark.carey@gmail.com> (careym)
- Matthieu Castet <matthieu.castet@parrot.com> (mat-c)
- nvidiaman <nvidiaman@users.sf.net> (nvidiaman)
- Patrice Mandin <patmandin@gmail.com> (pmandin, pmdata)
- Pekka Paalanen <pq@iki.fi> (pq, ppaalanen)
- Peter Popov <ironpeter@users.sf.net> (ironpeter)
- Richard Hughes <hughsient@users.sf.net> (hughsient)
- Rudi Cilibrasi <cilibrar@users.sf.net> (cilibrar)
- Serge Martin
- Simon Raffeiner
- Stephane Loeuillet <leroutier@users.sf.net> (leroutier)
- Stephane Marchesin <stephane.marchesin@gmail.com> (marcheu)
- sturmflut <sturmflut@users.sf.net> (sturmflut)
- Sylvain Munaut <tnt@246tNt.com>
- Victor Stinner <victor.stinner@haypocalc.com> (haypo)
- Wladmir van der Laan <laanwj@gmail.com> (miathan6)
- Younes Manton <younes.m@gmail.com> (ymanton)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#define NV01_ROOT						0x00000001
#define NV01_CONTEXT_DMA					0x00000002
#define NV01_DEVICE						0x00000003
#define NV01_TIMER						0x00000004
#define NV01_NULL						0x00000030
#define NV01_MEMORY_LOCAL_BANKED				0x0000003d
#define NV01_MAPPING_SYSTEM					0x0000003e
#define NV03_MEMORY_LOCAL_CURSOR				0x0000003f
#define NV01_MEMORY_LOCAL_LINEAR				0x00000040
#define NV01_MAPPING_LOCAL					0x00000041
#define NV03_VIDEO_LUT_CURSOR_DAC				0x00000046
#define NV03_CHANNEL_PIO					0x0000006a
#define NV03_CHANNEL_DMA					0x0000006b
#define NV10_VIDEO_DISPLAY					0x0000007c
#define NV01_CONTEXT_BETA1					0x00000012
#define NV04_BETA_SOLID						0x00000072
#define NV01_CONTEXT_COLOR_KEY					0x00000017
#define NV04_CONTEXT_COLOR_KEY					0x00000057
#define NV01_CONTEXT_PATTERN					0x00000018
#define NV01_CONTEXT_CLIP_RECTANGLE				0x00000019
#define NV03_CONTEXT_ROP					0x00000043
#define NV04_IMAGE_PATTERN					0x00000044
#define NV01_RENDER_SOLID_LINE					0x0000001c
#define NV04_RENDER_SOLID_LINE					0x0000005c
#define NV30_RENDER_SOLID_LINE					0x0000035c
#define NV40_RENDER_SOLID_LINE					0x0000305c
#define NV01_RENDER_SOLID_TRIANGLE				0x0000001d
#define NV04_RENDER_SOLID_TRIANGLE				0x0000005d
#define NV01_RENDER_SOLID_RECTANGLE				0x0000001e
#define NV04_RENDER_SOLID_RECTANGLE				0x0000005e
#define NV01_IMAGE_BLIT						0x0000001f
#define NV04_IMAGE_BLIT						0x0000005f
#define NV11_IMAGE_BLIT						0x0000009f
#define NV01_IMAGE_FROM_CPU					0x00000021
#define NV04_IMAGE_FROM_CPU					0x00000061
#define NV05_IMAGE_FROM_CPU					0x00000065
#define NV10_IMAGE_FROM_CPU					0x0000008a
#define NV30_IMAGE_FROM_CPU					0x0000038a
#define NV40_IMAGE_FROM_CPU					0x0000308a
#define NV03_STRETCHED_IMAGE_FROM_CPU				0x00000036
#define NV04_STRETCHED_IMAGE_FROM_CPU				0x00000076
#define NV05_STRETCHED_IMAGE_FROM_CPU				0x00000066
#define NV30_STRETCHED_IMAGE_FROM_CPU				0x00000366
#define NV40_STRETCHED_IMAGE_FROM_CPU				0x00003066
#define NV03_SCALED_IMAGE_FROM_MEMORY				0x00000037
#define NV04_SCALED_IMAGE_FROM_MEMORY				0x00000077
#define NV05_SCALED_IMAGE_FROM_MEMORY				0x00000063
#define NV10_SCALED_IMAGE_FROM_MEMORY				0x00000089
#define NV30_SCALED_IMAGE_FROM_MEMORY				0x00000389
#define NV40_SCALED_IMAGE_FROM_MEMORY				0x00003089
#define NV50_SCALED_IMAGE_FROM_MEMORY				0x00005089
#define NV04_DVD_SUBPICTURE					0x00000038
#define NV10_DVD_SUBPICTURE					0x00000088
#define NV03_GDI_RECTANGLE_TEXT					0x0000004b
#define NV04_GDI_RECTANGLE_TEXT					0x0000004a
#define NV04_SWIZZLED_SURFACE					0x00000052
#define NV20_SWIZZLED_SURFACE					0x0000009e
#define NV30_SWIZZLED_SURFACE					0x0000039e
#define NV40_SWIZZLED_SURFACE					0x0000309e
#define NV03_CONTEXT_SURFACE_DST				0x00000058
#define NV03_CONTEXT_SURFACE_SRC				0x00000059
#define NV04_CONTEXT_SURFACES_2D				0x00000042
#define NV10_CONTEXT_SURFACES_2D				0x00000062
#define NV30_CONTEXT_SURFACES_2D				0x00000362
#define NV40_CONTEXT_SURFACES_2D				0x00003062
#define NV50_CONTEXT_SURFACES_2D				0x00005062
#define NV04_INDEXED_IMAGE_FROM_CPU				0x00000060
#define NV05_INDEXED_IMAGE_FROM_CPU				0x00000064
#define NV30_INDEXED_IMAGE_FROM_CPU				0x00000364
#define NV40_INDEXED_IMAGE_FROM_CPU				0x00003064
#define NV10_TEXTURE_FROM_CPU					0x0000007b
#define NV30_TEXTURE_FROM_CPU					0x0000037b
#define NV40_TEXTURE_FROM_CPU					0x0000307b
#define NV04_M2MF						0x00000039
#define NV50_M2MF						0x00005039
#define NVC0_M2MF						0x00009039
#define NV03_TEXTURED_TRIANGLE					0x00000048
#define NV04_TEXTURED_TRIANGLE					0x00000054
#define NV10_TEXTURED_TRIANGLE					0x00000094
#define NV04_MULTITEX_TRIANGLE					0x00000055
#define NV10_MULTITEX_TRIANGLE					0x00000095
#define NV03_CONTEXT_SURFACE_COLOR				0x0000005a
#define NV03_CONTEXT_SURFACE_ZETA				0x0000005b
#define NV04_CONTEXT_SURFACES_3D				0x00000053
#define NV10_CONTEXT_SURFACES_3D				0x00000093
#define NV10_3D							0x00000056
#define NV11_3D							0x00000096
#define NV17_3D							0x00000099
#define NV20_3D							0x00000097
#define NV25_3D							0x00000597
#define NV30_3D							0x00000397
#define NV35_3D							0x00000497
#define NV34_3D							0x00000697
#define NV40_3D							0x00004097
#define NV44_3D							0x00004497
#define NV50_3D							0x00005097
#define NV84_3D							0x00008297
#define NVA0_3D							0x00008397
#define NVA3_3D							0x00008597
#define NVAF_3D							0x00008697
#define NVC0_3D							0x00009097
#define NV50_2D							0x0000502d
#define NVC0_2D							0x0000902d
#define NV50_COMPUTE						0x000050c0
#define NVA3_COMPUTE						0x000085c0
#define NVC0_COMPUTE						0x000090c0
#define NV84_CRYPT						0x000074c1
#define NV01_SUBCHAN__SIZE					0x00008000
#define NV01_SUBCHAN						0x00000000

#define NV01_SUBCHAN_OBJECT					0x00000000


#define NV84_SUBCHAN_QUERY_ADDRESS_HIGH				0x00000010

#define NV84_SUBCHAN_QUERY_ADDRESS_LOW				0x00000014

#define NV84_SUBCHAN_QUERY_SEQUENCE				0x00000018

#define NV84_SUBCHAN_QUERY_GET					0x0000001c

#define NV84_SUBCHAN_QUERY_INTR					0x00000020

#define NV84_SUBCHAN_WRCACHE_FLUSH				0x00000024

#define NV10_SUBCHAN_REF_CNT					0x00000050


#define NV11_SUBCHAN_DMA_SEMAPHORE				0x00000060

#define NV11_SUBCHAN_SEMAPHORE_OFFSET				0x00000064

#define NV11_SUBCHAN_SEMAPHORE_ACQUIRE				0x00000068

#define NV11_SUBCHAN_SEMAPHORE_RELEASE				0x0000006c

#define NV40_SUBCHAN_YIELD					0x00000080

#define NV01_GRAPH						0x00000000

#define NV04_GRAPH_NOP						0x00000100

#define NV04_GRAPH_NOTIFY					0x00000104
#define NV04_GRAPH_NOTIFY_WRITE					0x00000000
#define NV04_GRAPH_NOTIFY_WRITE_AND_AWAKEN			0x00000001

#define NVC0_GRAPH_NOTIFY_ADDRESS_HIGH				0x00000104

#define NVC0_GRAPH_NOTIFY_ADDRESS_LOW				0x00000108

#define NVC0_GRAPH_NOTIFY					0x0000010c
#define NVC0_GRAPH_NOTIFY_WRITE					0x00000000
#define NVC0_GRAPH_NOTIFY_WRITE_AND_AWAKEN			0x00000001

#define NV50_GRAPH_WAIT_FOR_IDLE				0x00000110

#define NVC0_GRAPH_MACRO_UPLOAD_POS				0x00000114

#define NVC0_GRAPH_MACRO_UPLOAD_DATA				0x00000118

#define NVC0_GRAPH_MACRO_ID					0x0000011c

#define NVC0_GRAPH_MACRO_POS					0x00000120

#define NVA3_GRAPH_UNK0120					0x00000120

#define NVA3_GRAPH_UNK0124					0x00000124

#define NV40_GRAPH_PM_TRIGGER					0x00000140


#endif /* NV_OBJECT_XML */
