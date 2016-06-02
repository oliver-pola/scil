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

/*
 * This tool will compress / decompress a CSV
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <scil.h>
#include <scil-util.h>
#include <scil-option.h>

static int validate = 0;
static int verbose = 0;
static int compress = 0;
static int uncompress = 0;
static int cycle = 0;
static char * in_file = "";
static char * out_file = "";
static char delim = ',';

static int data_type_float = 0;
static int measure_time = 0;
static int ignore_header = 0;
static int output_header = 0;

// data we process

static scil_dims dims;
static int datatype = SCIL_TYPE_DOUBLE;
static byte * input_data = NULL;
static byte * output_data = NULL;

void readCSVData(){
  FILE * fd = fopen(in_file, "r");
  if (! fd){
    printf("Error file %s does not exist\n", in_file);
    exit(1);
  }

  int ret = 1;
  double dbl;
  int x = 0;
  int y = 0;
  int ref_x = -1;

  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char delimeter[2];
  delimeter[0] = delim;
  delimeter[1] = 0;

  if (ignore_header){
    read = getline(&line, &len, fd);
  }

  while ((read = getline(&line, &len, fd)) != -1) {
    char * data = strtok(line, delimeter);
    x = 0;
    while( data != NULL ){
      // count the number of elements.
      ret = sscanf(data, "%lf", & dbl);
      data = strtok(NULL, delimeter);
      x++;
    }
    if (ref_x != -1 && x != ref_x && x > 1){
      printf("Error reading file %s, number of columns varies, saw %d, expected %d\n", in_file, x, ref_x);
      exit(1);
    }
    ref_x = x;
    if (x > 1){
      y++;
    }
  }
  fclose(fd);

  printf("Read file %s: %d %d\n", in_file, x, y);

  if(y > 1){
    scil_init_dims_2d(& dims, x, y);
  }else{
    scil_init_dims_1d(& dims, x);
  }

  input_data = (byte*) malloc(scil_compress_buffer_size_bound(datatype, & dims));

  fd = fopen(in_file, "r");
  if (ignore_header){
    read = getline(&line, &len, fd);
  }

  size_t pos = 0;
  while ((read = getline(&line, &len, fd)) != -1) {
    char * data = strtok(line, delimeter);
    x = 0;
    while( data != NULL ){
      // count the number of elements.
      ret = sscanf(data, "%lf", & dbl);
      if(data_type_float){
        ((float*) input_data)[pos] = (float) dbl;
      }else{
        ((double*) input_data)[pos] = dbl;
      }
      pos++;

      data = strtok(NULL, delimeter);
      x++;
    }
    ref_x = x;
    if (x > 1){
      y++;
    }
  }

  fclose(fd);
}



void writeCSVData(){
  FILE * f = fopen(out_file, "w");
  if(f == NULL){
    printf("Coult not open %s for write\n", out_file);
    exit(1);
  }
  double * buffer_in = (double *) output_data;
  if(dims.dims == 1){
    if (output_header)
      fprintf(f, "%zu\n", dims.length[0]);
    fprintf(f, "%.17f", buffer_in[0]);
    for(size_t x = 1; x < dims.length[0]; x++){
      fprintf(f, ",%.17f", buffer_in[x]);
    }
    fprintf(f, "\n");
  }else{
    if (output_header)
      fprintf(f, "%zu, %zu\n", dims.length[0], dims.length[1]);
    for(size_t y = 0; y < dims.length[1]; y++){
      fprintf(f, "%.17f", buffer_in[0+ y * dims.length[0]]);
      for(size_t x = 1; x < dims.length[0]; x++){
        fprintf(f, ",%.17f", buffer_in[x+ y * dims.length[0]]);
      }
      fprintf(f, "\n");
    }
  }
  fclose(f);

}

void readData(){

}

void writeData(){

}

int main(int argc, char ** argv){
  scil_context_p ctx;
  scil_hints hints;

  int ret;

  scil_init_hints(&hints);

  option_help known_args[] = {
    {'i', "in_file", "Input file (file format depends on mode)", OPTION_REQUIRED_ARGUMENT, 's', & in_file},
    {'o', "out_file", "Output file (file format depends on mode)", OPTION_REQUIRED_ARGUMENT, 's', & out_file},
    {'x', "decompress", "Infile is expected to be a binary compressed with this tool, outfile a CSV file",
      OPTION_FLAG, 'd', & uncompress},
    {'c', "compress", "Infile is expected to be a CSV file, outfile a binary",
      OPTION_FLAG, 'd' , & compress},
    {'f', "float", "Use float as datatype otherwise double.", OPTION_FLAG, 'd', & data_type_float},
    {'t', "time", "Measure time for the operation.", OPTION_FLAG, 'd', & measure_time},
    {'V', "validate", "Validate the output", OPTION_FLAG, 'd', & validate},
    {'v', "verbose", "Increase the verbosity level", OPTION_FLAG, 'd', & verbose},
    {'d', "delim", "Seperator", OPTION_OPTIONAL_ARGUMENT, 'c', & delim},
    {0, "hint-force_compression_methods", NULL,  OPTION_OPTIONAL_ARGUMENT, 's', & hints.force_compression_methods},
    {0, "hint-absolute-tolerance", NULL,  OPTION_OPTIONAL_ARGUMENT, 'f', & hints.absolute_tolerance},
    {0, "add-output-header", "Provide an header for plotting", OPTION_FLAG, 'd', & output_header},
    {0, "ignore-header", "Ignore the header", OPTION_FLAG, 'd', & ignore_header},
    {0, "cycle", "For testing: Compress, then decompress and store the output. Files are CSV files",
        OPTION_FLAG, 'd' , & cycle},
    {0, 0, 0, 0, 0, NULL}
  };

  scilO_parseOptions(argc, argv, known_args);

  if (data_type_float){
    datatype = SCIL_TYPE_FLOAT;
  }

  ret = scil_create_compression_context(& ctx, datatype, &hints);

  if (compress || cycle){
    readCSVData();
  }else{
    readData();
  }


  // TODO do compression, decompression
  output_data = input_data;

  // scil_compress_buffer_size_bound(datatype, & dims);

	scil_timer timer;
	scilU_start_timer(& timer);

	// ret = scil_compress(buffer_out, buff_size, buffer_in, & dims, & out_c_size, ctx);
  // ret = scil_decompress(datatype, buffer_uncompressed, & dims, buffer_out, out_c_size, tmp_buff);
	double runtime = scilU_stop_timer(timer);
  if(measure_time){
    printf("%fs \n", runtime);
  }

  if (uncompress || cycle){
    writeCSVData();
  }else{
    writeData();
  }

  return 0;
}
