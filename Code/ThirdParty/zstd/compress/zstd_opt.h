/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#ifndef ZSTD_OPT_H
#define ZSTD_OPT_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "zstd_compress_internal.h"

/* used in ZSTD_loadDictionaryContent() */
void ZSTD_updateTree(ZSTD_matchState_t* ms, const BYTE* ip, const BYTE* iend);

size_t ZSTD_compressBlock_btopt(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);
size_t ZSTD_compressBlock_btultra(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);
size_t ZSTD_compressBlock_btultra2(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);


size_t ZSTD_compressBlock_btopt_dictMatchState(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);
size_t ZSTD_compressBlock_btultra_dictMatchState(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);

size_t ZSTD_compressBlock_btopt_extDict(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);
size_t ZSTD_compressBlock_btultra_extDict(
        ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM],
        void const* src, size_t srcSize);

        /* note : no btultra2 variant for extDict nor dictMatchState,
         * because btultra2 is not meant to work with dictionaries
         * and is only specific for the first block (no prefix) */

#if defined (__cplusplus)
}
#endif

#endif /* ZSTD_OPT_H */
