#include "add_one.h"
#include "constants.h"
#include "graph.h"

constexpr auto BURST_LENGTH{64};
constexpr auto BANDWIDTH{1000};

SimpleGraph simple_graph{};

SimpleGraph::SimpleGraph() : m_kernel{adf::kernel::create(AddOne)},
                             m_gmio_out{adf::output_gmio::create(BURST_LENGTH, BANDWIDTH)},
                             m_gmio_in{adf::input_gmio::create(BURST_LENGTH, BANDWIDTH)}
{
  adf::connect(m_gmio_in.out[0], m_kernel.in[0]);
  adf::connect(m_kernel.out[0], m_gmio_out.in[0]);
  adf::source(m_kernel) = "add_one.cpp";

  adf::dimensions(m_kernel.in[0]) = {AIE_BUFFER_SIZE};
  adf::dimensions(m_kernel.out[0]) = {AIE_BUFFER_SIZE};

  adf::runtime(m_kernel) = 0.9;
}

#if defined(__AIESIM__) || defined(__X86SIM__)
int main()
{
  return 0;
}
#endif
