#include "engine/algorithms/xrandom.h"

#include <cstdlib>

float XEngine::XRandom::GetFloat(float fRangeStart /* = 0.0f */, float fRangeEnd /* = 1.0f */)
{
	float fRand = rand() / (float)RAND_MAX;
	fRand *= (fRangeEnd - fRangeStart);
	fRand += fRangeStart;
	return fRand;
}

int XEngine::XRandom::GetInt( int nRangeStart, int nRangeEnd )
{
	int nRandom = rand();
	int nNumPossibleResults = nRangeEnd - nRangeStart;
	int nNumResultsToReject = RAND_MAX % nNumPossibleResults;
	if (RAND_MAX - nRandom < nNumResultsToReject)  ///This should be a "while" for perfectly even distribution, but even checking once is probably overkill. 
	{
		nRandom = rand();
	}
	return nRangeStart + (nRandom / nNumPossibleResults);
}