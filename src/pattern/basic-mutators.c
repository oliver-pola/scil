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

#include <basic-mutators.h>
#include <scil-util.h>

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"


typedef struct interpolator_data_1D {
  int dim; // current dim
  int * stride;
  int l; // interpolation length
} interpolator_data_1D_t;

static void m_interpolator_func(double* data, const scil_dims* pos, const scil_dims* size, int* iter, const void* user_ptr)
{
  interpolator_data_1D_t* up = (interpolator_data_1D_t*) user_ptr;
  const int d = up->dim;
  const int l = up->l;

  scil_dims start;
  scil_dims end;
  scil_copy_dims_array(& end, *pos);
  scil_copy_dims_array(& start, *pos);

  start.length[d] = start.length[d] / l * l;
  end.length[d] = (end.length[d]) / l * l + l;

  if(end.length[d] >= size->length[d]){
    end.length[d] = size->length[d] - 1;
  }

  double weight = ((double) pos->length[d] - start.length[d]) / (end.length[d] - start.length[d]);

  //scilU_print_dims(pos);
  //scilU_print_dims(start);
  //scilU_print_dims(end);

  double start_v = data[scilG_data_pos(&start, size)];
  double end_v =   data[scilG_data_pos(&end, size)];

  // use cosine interpolation
  weight = (1-cos(weight*M_PI))/2;
  double val = (1.0-weight) * start_v + weight * end_v;
  //printf("%.2f %.1f \n", weight, val);
  data[scilG_data_pos(pos, size)] = val;
}

static void m_interpolator(double* data, const scil_dims* dims, float arg){
  const int l = (int) arg;
  assert(l > 1);

  scil_dims ende;
  scil_copy_dims_array(& ende, *dims);
  scil_dims final;
  scil_copy_dims_array(& final, *dims);

  scil_dims pos;
  scil_copy_dims_array(& pos, *dims);
  memset(pos.length, 0, sizeof(size_t)*pos.dims);

  int stride[pos.dims];
  for(int i=0; i < pos.dims; i++){
    stride[i] = l;
    final.length[i]--;
  }

  for(int i = pos.dims - 1 ; i >= 0; i--){
    stride[i] = 1;

    interpolator_data_1D_t inter_data = {i, stride, l};
    scilG_iter(data, dims, &pos, &final, stride, & m_interpolator_func, & inter_data );

    for(int j=0; j < pos.dims; j++){
      ende.length[j] = dims->length[j] - 1;
    }
    ende.length[i] = 0;
    scilG_iter(data, dims, &ende, dims, stride, & m_interpolator_func, & inter_data );
  }
}

scilPa_mutator scilPa_interpolator = &m_interpolator;

static void m_repeater_func_i(double* data, const scil_dims* pos, const scil_dims* size, int* iter, const void* user_ptr){
  data[scilG_data_pos(pos, size)] = *((double*) user_ptr);
}

static void m_repeater_func(double* data, const scil_dims* pos, const scil_dims* size, int* iter, const void* user_ptr){
  int l = *((int *) user_ptr);

  double val = data[scilG_data_pos(pos, size)];
  scil_dims extend;
  for(int j=0; j < pos->dims; j++){
    if (pos->length[j] + l < size->length[j] ){
      extend.length[j] = pos->length[j] + l;
    }else{
      extend.length[j] = size->length[j];
    }
  }
  scilG_iter(data, size, pos, &extend, NULL, & m_repeater_func_i, & val );
}

static void m_repeater(double* data, const scil_dims* dims, float arg){
  int l = (int) arg;
  assert(l > 1);

  scil_dims pos;
  scil_copy_dims_array(& pos, *dims);
  memset(pos.length, 0, sizeof(size_t)*pos.dims);

  int stride[pos.dims];
  for(int i=0; i < pos.dims; i++){
    stride[i] = l;
  }

  scilG_iter(data, dims, &pos, dims, stride, & m_repeater_func, & l );
}

scilPa_mutator scilPa_repeater = & m_repeater;
