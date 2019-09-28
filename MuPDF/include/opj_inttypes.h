/*
 * The copyright in this software is being made available under the 2-clauses 
 * BSD License, included below. This software may be subject to other third 
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2012, Mathieu Malaterre <mathieu.malaterre@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef OPJ_INTTYPES_H
#define OPJ_INTTYPES_H

//#include "opj_config_private.h"
#ifdef OPJ_HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if defined(_WIN32)
#define PRId64 "I64d"
#define PRIi64 "I64i"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define 	PRId8   "d"
#define 	PRIdLEAST8   "d"
#define 	PRIdFAST8   "d"
#define 	PRIi8   "i"
#define 	PRIiLEAST8   "i"
#define 	PRIiFAST8   "i"
#define 	PRId16   "d"
#define 	PRIdLEAST16   "d"
#define 	PRIdFAST16   "d"
#define 	PRIi16   "i"
#define 	PRIiLEAST16   "i"
#define 	PRIiFAST16   "i"
#define 	PRId32   "ld"
#define 	PRIdLEAST32   "ld"
#define 	PRIdFAST32   "ld"
#define 	PRIi32   "li"
#define 	PRIiLEAST32   "li"
#define 	PRIiFAST32   "li"
#define 	PRIdPTR   PRId16
#define 	PRIiPTR   PRIi16
#define 	PRIo8   "o"
#define 	PRIoLEAST8   "o"
#define 	PRIoFAST8   "o"
#define 	PRIu8   "u"
#define 	PRIuLEAST8   "u"
#define 	PRIuFAST8   "u"
#define 	PRIx8   "x"
#define 	PRIxLEAST8   "x"
#define 	PRIxFAST8   "x"
#define 	PRIX8   "X"
#define 	PRIXLEAST8   "X"
#define 	PRIXFAST8   "X"
#define 	PRIo16   "o"
#define 	PRIoLEAST16   "o"
#define 	PRIoFAST16   "o"
#define 	PRIu16   "u"
#define 	PRIuLEAST16   "u"
#define 	PRIuFAST16   "u"
#define 	PRIx16   "x"
#define 	PRIxLEAST16   "x"
#define 	PRIxFAST16   "x"
#define 	PRIX16   "X"
#define 	PRIXLEAST16   "X"
#define 	PRIXFAST16   "X"
#define 	PRIo32   "lo"
#define 	PRIoLEAST32   "lo"
#define 	PRIoFAST32   "lo"
#define 	PRIu32   "lu"
#define 	PRIuLEAST32   "lu"
#define 	PRIuFAST32   "lu"
#define 	PRIx32   "lx"
#define 	PRIxLEAST32   "lx"
#define 	PRIxFAST32   "lx"
#define 	PRIX32   "lX"
#define 	PRIXLEAST32   "lX"
#define 	PRIXFAST32   "lX"
#define 	PRIoPTR   PRIo16
#define 	PRIuPTR   PRIu16
#define 	PRIxPTR   PRIx16
#define 	PRIXPTR   PRIX16
#define 	SCNd8   "hhd"
#define 	SCNdLEAST8   "hhd"
#define 	SCNdFAST8   "hhd"
#define 	SCNi8   "hhi"
#define 	SCNiLEAST8   "hhi"
#define 	SCNiFAST8   "hhi"
#define 	SCNd16   "d"
#define 	SCNdLEAST16   "d"
#define 	SCNdFAST16   "d"
#define 	SCNi16   "i"
#define 	SCNiLEAST16   "i"
#define 	SCNiFAST16   "i"
#define 	SCNd32   "ld"
#define 	SCNdLEAST32   "ld"
#define 	SCNdFAST32   "ld"
#define 	SCNi32   "li"
#define 	SCNiLEAST32   "li"
#define 	SCNiFAST32   "li"
#define 	SCNdPTR   SCNd16
#define 	SCNiPTR   SCNi16
#define 	SCNo8   "hho"
#define 	SCNoLEAST8   "hho"
#define 	SCNoFAST8   "hho"
#define 	SCNu8   "hhu"
#define 	SCNuLEAST8   "hhu"
#define 	SCNuFAST8   "hhu"
#define 	SCNx8   "hhx"
#define 	SCNxLEAST8   "hhx"
#define 	SCNxFAST8   "hhx"
#define 	SCNo16   "o"
#define 	SCNoLEAST16   "o"
#define 	SCNoFAST16   "o"
#define 	SCNu16   "u"
#define 	SCNuLEAST16   "u"
#define 	SCNuFAST16   "u"
#define 	SCNx16   "x"
#define 	SCNxLEAST16   "x"
#define 	SCNxFAST16   "x"
#define 	SCNo32   "lo"
#define 	SCNoLEAST32   "lo"
#define 	SCNoFAST32   "lo"
#define 	SCNu32   "lu"
#define 	SCNuLEAST32   "lu"
#define 	SCNuFAST32   "lu"
#define 	SCNx32   "lx"
#define 	SCNxLEAST32   "lx"
#define 	SCNxFAST32   "lx"
#define 	SCNoPTR   SCNo16
#define 	SCNuPTR   SCNu16
#define 	SCNxPTR   SCNx16
#else
#error unsupported platform
#endif
#endif

#endif /* OPJ_INTTYPES_H */
