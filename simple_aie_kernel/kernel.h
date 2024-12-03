#pragma once

#include <adf.h>

void AddOne(adf::input_buffer<int32, adf::extents<256>> &restrict data_in, adf::output_buffer<int32, adf::extents<256>> &restrict data_out);
