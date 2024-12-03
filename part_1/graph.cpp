#include "graph.h"
#include "kernel.h"

constexpr auto BURST_LENGTH{64};
constexpr auto BANDWIDTH{1000};

SimpleGraph::SimpleGraph() : m_kernel{adf::kernel::create(AddOne)},
                             m_gmio_out{adf::output_gmio::create(BURST_LENGTH, BANDWIDTH)},
                             m_gmio_in{adf::input_gmio::create(BURST_LENGTH, BANDWIDTH)}
{
  adf::connect(m_gmio_in.out[0], m_kernel.in[0]);
  adf::connect(m_kernel.out[0], m_gmio_out.in[0]);
  adf::source(m_kernel) = "add_one.cpp";
  adf::runtime(m_kernel) = 0.9;
}

SimpleGraph simple_graph{};

#if defined(__AIESIM__) || defined(__X86SIM__)
int main()
{
  constexpr auto ITERATION{4};
  constexpr auto BLOCK_BYTES{ITERATION * AIE_BUFFER_SIZE * sizeof(AIE_BUFFER_TYPE)};
  auto data_in{(AIE_BUFFER_TYPE *)adf::GMIO::malloc(BLOCK_BYTES)};
  auto data_out{(AIE_BUFFER_TYPE *)adf::GMIO::malloc(BLOCK_BYTES)};

  std::cout << "GMIO malloc completed\n";

  AIE_BUFFER_TYPE data_value{};
  for (auto gmio_index{0}; gmio_index < ITERATION * AIE_BUFFER_SIZE; ++gmio_index)
  {
    data_in[gmio_index] = data_value;
    ++data_value;
  }

  simple_graph.init();
  simple_graph.m_gmio_in.gm2aie_nb(data_in, BLOCK_BYTES);
  simple_graph.run(ITERATION);
  simple_graph.m_gmio_out.aie2gm(data_out, BLOCK_BYTES);
  simple_graph.end();

  auto errors{0};
  for (auto ii{0}; ii < ITERATION * AIE_BUFFER_SIZE; ++ii)
  {
    if (data_out[ii] != data_in[ii] + 1)
    {
      std::cout << "Error: dout[" << ii << "] = " << data_out[ii] << ", gold = " << data_in[ii] + 1 << '\n';
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
