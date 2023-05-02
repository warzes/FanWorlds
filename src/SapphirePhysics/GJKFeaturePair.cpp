#include "stdafx.h"
#include "GJK.h"

GJKFeaturePair GetFeaturePair(const SimplexCache& cache)
{
	assert(0 < cache.count && cache.count < 4);

	uint32_t vertexCount = cache.count;
	uint32_t uniqueCount1 = UniqueCount(cache.index1, vertexCount);
	uint32_t uniqueCount2 = UniqueCount(cache.index2, vertexCount);

	if (vertexCount == 1)
	{
		// VV 
		GJKFeaturePair pair;
		pair.count1 = 1;
		pair.count2 = 1;
		pair.index1[0] = cache.index1[0];
		pair.index2[0] = cache.index2[0];
		return pair;
	}

	if (vertexCount == 2)
	{
		if (uniqueCount1 == 2 && uniqueCount2 == 2)
		{
			// EE
			GJKFeaturePair pair;
			pair.count1 = 2;
			pair.count2 = 2;
			pair.index1[0] = cache.index1[0];
			pair.index1[1] = cache.index1[1];
			pair.index2[0] = cache.index2[0];
			pair.index2[1] = cache.index2[1];
			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index2[0] != pair.index2[1]);
			return pair;
		}

		if (uniqueCount1 == 1)
		{
			// VE
			GJKFeaturePair pair;
			pair.count1 = 1;
			pair.count2 = 2;
			pair.index1[0] = cache.index1[0];
			pair.index2[0] = cache.index2[0];
			pair.index2[1] = cache.index2[1];
			assert(pair.index2[0] != pair.index2[1]);
			return pair;
		}

		if (uniqueCount2 == 1)
		{
			// EV
			GJKFeaturePair pair;
			pair.count1 = 2;
			pair.count2 = 1;
			pair.index1[0] = cache.index1[0];
			pair.index1[1] = cache.index1[1];
			pair.index2[0] = cache.index2[0];
			assert(pair.index1[0] != pair.index1[1]);
			return pair;
		}
		assert(false);
	}

	if (vertexCount == 3)
	{
		if (uniqueCount1 == 1 && uniqueCount2 == 3)
		{
			// VF
			GJKFeaturePair pair;
			pair.count1 = 1;
			pair.count2 = 3;
			pair.index1[0] = cache.index1[0];
			pair.index2[0] = cache.index2[0];
			pair.index2[1] = cache.index2[1];
			pair.index2[2] = cache.index2[2];
			assert(pair.index2[0] != pair.index2[1]);
			assert(pair.index2[1] != pair.index2[2]);
			assert(pair.index2[2] != pair.index2[0]);
			return pair;
		}

		if (uniqueCount1 == 3 && uniqueCount2 == 1)
		{
			// FV
			GJKFeaturePair pair;
			pair.count1 = 3;
			pair.count2 = 1;
			pair.index1[0] = cache.index1[0];
			pair.index1[1] = cache.index1[1];
			pair.index1[2] = cache.index1[2];
			pair.index2[0] = cache.index2[0];
			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index1[1] != pair.index1[2]);
			assert(pair.index1[2] != pair.index1[0]);
			return pair;
		}

		if (uniqueCount1 == 2 && uniqueCount2 == 2)
		{
			// EE
			GJKFeaturePair pair;
			pair.count1 = 2;
			pair.count2 = 2;
			pair.index1[0] = cache.index1[0];
			pair.index2[0] = cache.index2[0];

			if (cache.index1[0] == cache.index1[1])
			{
				pair.index1[1] = cache.index1[2];
			}
			else
			{
				pair.index1[1] = cache.index1[1];
			}

			if (cache.index2[0] == cache.index2[1])
			{
				pair.index2[1] = cache.index2[2];
			}
			else
			{
				pair.index2[1] = cache.index2[1];
			}
			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index2[0] != pair.index2[1]);
			return pair;
		}

		if (uniqueCount1 == 2 && uniqueCount2 == 3)
		{
			// EF
			GJKFeaturePair pair;
			pair.count1 = 2;
			pair.count2 = 3;

			pair.index1[0] = cache.index1[0];
			if (cache.index1[0] == cache.index1[1])
			{
				pair.index1[1] = cache.index1[2];
			}
			else
			{
				pair.index1[1] = cache.index1[1];
			}

			pair.index2[0] = cache.index2[0];
			pair.index2[1] = cache.index2[1];
			pair.index2[2] = cache.index2[2];

			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index2[0] != pair.index2[1]);
			assert(pair.index2[1] != pair.index2[2]);
			assert(pair.index2[2] != pair.index2[0]);
			return pair;
		}

		if (uniqueCount1 == 3 && uniqueCount2 == 2)
		{
			// FE
			GJKFeaturePair pair;
			pair.count1 = 3;
			pair.count2 = 2;
			pair.index1[0] = cache.index1[0];
			pair.index1[1] = cache.index1[1];
			pair.index1[2] = cache.index1[2];

			pair.index2[0] = cache.index2[0];
			if (cache.index2[0] == cache.index2[1])
			{
				pair.index2[1] = cache.index2[2];
			}
			else
			{
				pair.index2[1] = cache.index2[1];
			}

			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index1[1] != pair.index1[2]);
			assert(pair.index1[2] != pair.index1[0]);
			assert(pair.index2[0] != pair.index2[1]);
			return pair;
		}

		if (uniqueCount1 == 3 && uniqueCount2 == 3)
		{
			// FF
			GJKFeaturePair pair;
			pair.count1 = 3;
			pair.count2 = 3;
			pair.index1[0] = cache.index1[0];
			pair.index1[1] = cache.index1[1];
			pair.index1[2] = cache.index1[2];
			pair.index2[0] = cache.index2[0];
			pair.index2[1] = cache.index2[1];
			pair.index2[2] = cache.index2[2];

			assert(pair.index1[0] != pair.index1[1]);
			assert(pair.index1[1] != pair.index1[2]);
			assert(pair.index1[2] != pair.index1[0]);

			assert(pair.index2[0] != pair.index2[1]);
			assert(pair.index2[1] != pair.index2[2]);
			assert(pair.index2[2] != pair.index2[0]);
			return pair;
		}

		assert(false);
	}

	assert(false);

	GJKFeaturePair pair;
	pair.count1 = 0;
	pair.count2 = 0;
	return pair;
}