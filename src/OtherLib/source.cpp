#include "stdafx.h"
#include "MultiArray.h"
#include "MultiVector.h"

void Foo()
{
	{
		MultiArray<float, dimensions<4, 4, 4>> multi_array;

		// Access.
		for( std::size_t x = 0; x < multi_array.dimensions()[0]; ++x )
			for( std::size_t y = 0; y < multi_array.dimensions()[1]; ++y )
				for( std::size_t z = 0; z < multi_array.dimensions()[2]; ++z )
					multi_array(x, y, z) = static_cast<float>(x + y + z);

		// Linear access.
		for( std::size_t i = 0; i < multi_array.size(); ++i )
			multi_array[i] = static_cast<float>(i);

		// Linear iteration.
		std::for_each(multi_array.begin(), multi_array.end(), [](auto& value) { value += 42.0f; });
	}

	{
		MultiVector<float, 3> multi_vector({ 4, 4, 4 }, 0.0f);

		// Resize.
		multi_vector.resize({ 8, 8, 8 });

		// Access.
		for( std::size_t x = 0; x < multi_vector.dimensions()[0]; ++x )
			for( std::size_t y = 0; y < multi_vector.dimensions()[1]; ++y )
				for( std::size_t z = 0; z < multi_vector.dimensions()[2]; ++z )
					multi_vector(x, y, z) = static_cast<float>(x + y + z);

		// Linear access.
		for( std::size_t i = 0; i < multi_vector.size(); ++i )
			multi_vector[i] = static_cast<float>(i);

		// Linear iteration.
		std::for_each(multi_vector.begin(), multi_vector.end(), [](auto& value) { value += 42.0f; });
	}
}