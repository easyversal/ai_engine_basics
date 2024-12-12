#include "../aie_component/constants.h"
#include "../aie_component/add_one.h"

#include <xrt/xrt_aie.h>

int main(int argc, char **argv)
{

    constexpr auto DEVICE_INDEX{0};
    auto device{xrt::device(DEVICE_INDEX)};

    const std::string xclbin_path{argv[1]};
    const auto uuid{device.load_xclbin(xclbin_path)};

    auto simple_graph{xrt::graph(device, uuid, "simple_graph")};

    constexpr auto BLOCK_BYTES{AIE_BUFFER_SIZE * sizeof(AIE_BUFFER_TYPE)};
    constexpr auto MEMORY_GROUP{0};

    auto din_buffer{xrt::aie::bo(device, BLOCK_BYTES, xrt::bo::flags::normal, MEMORY_GROUP)};
    auto data_in{din_buffer.map<int *>()};

    auto dout_buffer{xrt::aie::bo(device, BLOCK_BYTES, xrt::bo::flags::normal, MEMORY_GROUP)};
    auto data_out{dout_buffer.map<int *>()};

    std::cout << "Global memory allocation completed\n";

    AIE_BUFFER_TYPE data_value{};
    for (auto gmio_index{0}; gmio_index < AIE_BUFFER_SIZE; ++gmio_index)
    {
        data_in[gmio_index] = data_value;
        ++data_value;
    }

    constexpr auto OFFSET{0};
    din_buffer.sync("simple_graph.m_gmio_in", XCL_BO_SYNC_BO_GMIO_TO_AIE, BLOCK_BYTES, OFFSET);
    simple_graph.run(1);
    dout_buffer.sync("simple_graph.m_gmio_out", XCL_BO_SYNC_BO_AIE_TO_GMIO, BLOCK_BYTES, OFFSET);
    simple_graph.end();

    auto errors{0};
    for (auto ii{0}; ii < AIE_BUFFER_SIZE; ++ii)
    {
        if (data_out[ii] != data_in[ii] + 1)
        {
            std::cout << "Error: data_out[" << ii << "] = " << data_out[ii] << ", expected = " << data_in[ii] + 1 << '\n';
            ++errors;
        }
    }

    std::cout << "GMIO transactions finished\n";

    if (errors == 0)
    {
        std::cout << "TEST PASSED!\n";
    }
    else
    {
        std::cout << "ERRORS!\n";
    }

    return errors;
};
