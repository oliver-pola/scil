#include <string.h>

#include <scil-internal.h>
#include <util.h>


// known algorithms:
#include <algo/memcopy.h>
#include <algo/algo-1.h>

int scil_create_compression_context(scil_context ** out_ctx, scil_hints * hints){
	scil_context * ctx =(scil_context*)SAFE_MALLOC(sizeof(scil_context));
	*out_ctx = ctx;

	ctx->hints.relative_tolerance_percent = hints->relative_tolerance_percent;
	ctx->hints.absolute_tolerance = hints->absolute_tolerance;

	return 0;
}

int scil_compress(scil_context* ctx, char** compressed_buf_out, size_t* in_out_size, const double* data_in, const size_t in_size){
	assert(ctx != NULL);
	assert(data_in != NULL);

	int ret;

	const scil_hints * hints = & ctx->hints;

	// pick the best algorithm for the settings given in ctx...
	// now only one algorithm

	scil_compression_algorithm * last_algorithm;

	if (hints->absolute_tolerance == 0.0 || (hints->relative_err_finest_abs_tolerance == 0.0 || hints->relative_tolerance_percent == 0.0 || hints->significant_digits > 20) ){
		// we cannot compress because data must be accurate!
		last_algorithm = & algo_algo1;
	}else{
		last_algorithm = & algo_algo1;
	}
	ctx->last_algorithm = last_algorithm;

	// TODO add magic number to the output stream

	return last_algorithm->compress(ctx, compressed_buf_out, in_out_size, data_in, in_size);
}

int scil_decompress(const scil_context* ctx, double* data_out, size_t* in_out_size, const char* compressed_buf_in, const size_t in_size){
	assert(ctx != NULL);
	assert(in_out_size != NULL);

	int ret;

	// TODO check magic number from the output stream

	// TODO Assign decompressor according to the magic number

	// TODO decompress
	ret = scil_algo1_decompress(ctx, data_out, in_out_size, compressed_buf_in, in_size);
	return ret;
}

int scil_validate_compression(const scil_context* ctx,
                             const size_t uncompressed_size,
                             const double*restrict data_uncompressed,
                             const size_t compressed_size,
                             const double*restrict data_compressed )
{
	return 1;
}
