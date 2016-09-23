#ifndef SCIL_ALGOS_H
#define SCIL_ALGOS_H

// known algorithms:
#include <algo/algo-abstol.h>
#include <algo/algo-fpzip.h>
#include <algo/algo-gzip.h>
#include <algo/algo-memcopy.h>
#include <algo/algo-sigbits.h>
#include <algo/algo-zfp-abstol.h>
#include <algo/algo-zfp-precision.h>
#include <algo/lz4fast.h>
#include <algo/precond-dummy.h>
#include <algo/algo-quantize.h>
#include <algo/algo-swage.h>
#include <algo/algo-wavelets.h>

#include <scil-compression-algorithm.h>

extern scil_compression_algorithm_t* algo_array[];

/*
 \brief Returns the number of available compression schemes.
 */
int scilU_get_available_compressor_count();

const char* scilU_get_compressor_name(int number);

int scilU_get_compressor_number(const char* const name);

#endif // SCIL_ALGOS_H