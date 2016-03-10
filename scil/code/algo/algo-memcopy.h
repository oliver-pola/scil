// This file is part of SCIL.
//
// SCIL is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SCIL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with SCIL.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SCIL_ALGO_MEMCOPY_H_
#define SCIL_ALGO_MEMCOPY_H_

#include <scil-internal.h>

/**
 * \brief Trivial memcopy compression function
 * \param ctx Compression context used for this compression
 * \param dest Pre allocated buffer which will hold the compressed data
 * \param dest_size Byte size the compressed buffer will have
 * \param source Uncompressed data which should be processed
 * \param source_size Byte size of uncompressed buffer
 * \return Success state of the compression
 */
int scil_memcopy_compress(const scil_context* ctx, byte* restrict dest, uint64_t * restrict out_size, const byte*restrict source, const size_t source_size);

/**
 * \brief Trivial memcopy decompression function
 * \param ctx Compression context used for this compression
 * \param dest Pre allocated buffer which will hold the compressed data
 * \param dest_size Byte size the compressed buffer will have
 * \param source Uncompressed data which should be processed
 * \param source_size Byte size of uncompressed buffer
 * \return Success state of the compression
 */
int scil_memcopy_decompress(const scil_context* ctx, byte*restrict dest, uint64_t exp_size, const byte*restrict source, const size_t source_size);

extern scil_compression_algorithm algo_memcopy;

#endif