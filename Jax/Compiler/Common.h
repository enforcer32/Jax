#pragma once

#include <string>

namespace JAX
{
	struct Position
	{
		uint32_t Line;
		uint32_t Col;
		std::string FileName;
	};
}
