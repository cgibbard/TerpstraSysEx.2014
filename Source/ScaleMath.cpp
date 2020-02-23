/*
  ==============================================================================

    ScaleMath.cpp
    Created: 22 Feb 2020 7:13:35pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "ScaleMath.h"

static int modulo(int numIn, int mod)
{
	if (mod != 0)
		return ((numIn % mod) + mod) % mod;

	return 0;
}

// assuming input is positive
static Array<int> getPrimeFactorization(int numIn)
{
	Array<int> factorsout;

	if (numIn > 1)
	{
		int n = numIn;
		float q = numIn;
		bool doLoop;

		// n gets reduced as we find more factors
		for (int i = 0; i < NUMPRIMES; i++)
		{
			if (PRIMES[i] > n)
				break;

			factorsout.add(0);

			if (PRIMES[i] == n)
			{
				factorsout.getReference(i)++;
				break;
			}

			doLoop = true;

			while (doLoop)
			{
				q = n / (float)PRIMES[i];
				if (q == (int)q) {
					n = q;
					factorsout.getReference(i)++;
					continue;
				}
				doLoop = false;
			}
		}
	}

	return factorsout;
}

// assumes input is positive
static int getGCD(int num1, int num2)
{
	if (num1 == 0 || num2 == 0)
		return num1 + num2;
	else if (num1 == 1 || num2 == 1)
		return 1;
	else if (num1 == num2)
		return num1;

	return getGCD(num2, num1 % num2);
}

static int getLCM(int num1, int num2)
{
	long gcd = getGCD(num1, num2);
	return (int)(jmax(num1, num2) / gcd * (jmin(num1, num2)));
}

static Array<int> getCoprimes(int numIn, bool firstHalf)
{
	Array<int> coprimes = { 1 };
	int div, mod;
	int max = firstHalf ? (numIn / 2 + 1) : numIn - 1;
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

	coprimes.add(numIn - 1);
	return coprimes;
}

static Array<int> getContinuedFraction(double fractionIn, int maxDepth, double round0Thresh)
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
	}

	return cf;
}

static Array<int> getContinuedFraction(int numerator, int denominator, int maxDepth, double round0Thresh)
{
	return getContinuedFraction((double)numerator / denominator, maxDepth, round0Thresh);
}