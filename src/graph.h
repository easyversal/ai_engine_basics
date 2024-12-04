#pragma once

#include <adf.h>

class SimpleGraph : public adf::graph
{
public:
	SimpleGraph();

	adf::input_gmio m_gmio_in;
	adf::output_gmio m_gmio_out;

private:
	adf::kernel m_kernel;
};
