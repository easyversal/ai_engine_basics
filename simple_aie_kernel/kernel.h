#pragma once

#include <adf.h>

constexpr auto AIE_BUFFER_SIZE{256};
using AIE_BUFFER_TYPE = int32;

void AddOne(adf::input_buffer<AIE_BUFFER_TYPE, adf::extents<AIE_BUFFER_SIZE>> &restrict data_in, adf::output_buffer<AIE_BUFFER_TYPE, adf::extents<AIE_BUFFER_SIZE>> &restrict data_out);
