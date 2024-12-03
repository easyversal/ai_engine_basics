#include "constants.h"
#include "kernel.h"

#include <aie_api/aie.hpp>

void AddOne(adf::input_buffer<AIE_BUFFER_TYPE> &restrict data_in, adf::output_buffer<AIE_BUFFER_TYPE> &restrict data_out)
{
        auto in_iter{aie::begin(data_in)};
        auto out_iter{aie::begin(data_out)};

        for (auto ii{0}; ii < AIE_BUFFER_SIZE; ++ii)
        {
                const auto data_value{*in_iter};
                ++in_iter;

                const auto new_value{data_value + 1};
                *out_iter = new_value;
                ++out_iter;
        }
}
