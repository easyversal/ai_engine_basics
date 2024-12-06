#include "add_one.h"
#include "constants.h"
#include "graph.h"

SimpleGraph simple_graph{};

constexpr auto BURST_LENGTH{64};
constexpr auto BANDWIDTH{1000};

SimpleGraph::SimpleGraph() : m_kernel{adf::kernel::create(AddOne)},
                             m_gmio_out{adf::output_gmio::create(BURST_LENGTH, BANDWIDTH)},
                             m_gmio_in{adf::input_gmio::create(BURST_LENGTH, BANDWIDTH)}
{
  adf::source(m_kernel) = "add_one.cpp";
  adf::dimensions(m_kernel.in[0]) = {AIE_BUFFER_SIZE};
  adf::dimensions(m_kernel.out[0]) = {AIE_BUFFER_SIZE};
  adf::runtime(m_kernel) = 0.9;

  adf::connect(m_gmio_in.out[0], m_kernel.in[0]);
  adf::connect(m_kernel.out[0], m_gmio_out.in[0]);
}

#if defined(__AIESIM__) || defined(__X86SIM__)
int main()
{
  constexpr auto BLOCK_BYTES{AIE_BUFFER_SIZE * sizeof(AIE_BUFFER_TYPE)};
  auto data_in{(AIE_BUFFER_TYPE *)adf::GMIO::malloc(BLOCK_BYTES)};
  auto data_out{(AIE_BUFFER_TYPE *)adf::GMIO::malloc(BLOCK_BYTES)};

  std::cout << "Global memory allocation completed\n";

  AIE_BUFFER_TYPE data_value{};
  for (auto gmio_index{0}; gmio_index < AIE_BUFFER_SIZE; ++gmio_index)
  {
    data_in[gmio_index] = data_value;
    ++data_value;
  }

  simple_graph.init();
  simple_graph.m_gmio_in.gm2aie(data_in, BLOCK_BYTES);
  simple_graph.run(1);
  simple_graph.m_gmio_out.aie2gm(data_out, BLOCK_BYTES);
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

  adf::GMIO::free(data_in);
  adf::GMIO::free(data_out);

  if (errors == 0)
  {
    std::cout << "TEST PASSED!\n";
  }
  else
  {
    std::cout << "ERRORS!\n";
  }

  return errors;
}
#endif
