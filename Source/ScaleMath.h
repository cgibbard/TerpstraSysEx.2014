/*
  ==============================================================================

    ScaleMath.h
    Created: 22 Feb 2020 7:13:35pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

const int CF_MAXDEPTH = 20;
const double CF_ROUND0THRESH = 10e-8;

/* 
	Returns the Euclidian version of modulo, where all results are positive
	ie. -7 % 5 = 3 
*/
static int modulo(int numIn, int mod)
{
	if (mod != 0)
		return ((numIn % mod) + mod) % mod;

	return 0;
}

static Array<int> getCoprimes(int numIn, bool onlyFirstHalf=false)
{
	Array<int> coprimes = { 1 };
	int div, mod;
	int max = onlyFirstHalf ? (numIn / 2 + 1) : numIn;
	for (int i = 2; i < max; i++)
	{
		div = numIn;
		mod = i;
		while (mod > 1)
		{
			div %= mod;
			div += mod;
			mod = div - mod;
			div = div - mod;
		}

		if (mod > 0)
		{
			coprimes.add(i);
		}
	}

	return coprimes;
}

/*
	Returns an array of digits representing the continued fraction expansion of a given proportion
*/
static Array<int> getContinuedFraction(double fractionIn, int maxDepth = CF_MAXDEPTH, double round0Thresh = CF_ROUND0THRESH)
{
	Array<int> cf;
	double f = fractionIn;
	double nextF;

	for (int i = 0; i < maxDepth; i++)
	{
		cf.add((int)f);
		nextF = f - cf[i];
		if (nextF > round0Thresh)
			f = 1.0 / nextF;
		else
			break;
	}

	return cf;
}