#ifndef CPT_RAND_UTIL_H
#define CPT_RAND_UTIL_H

#include <string>

#include <Config.h>

// Returns a random numbuer in range[0, kuint64max]. Thread-safe.
uint64_t RandUint64();

uint64_t RandGenerator(uint64_t range);

// Retruns a random number between min and max (inclusive). Thread-safe.
int RandInt(int min, int max);

// Fill |len| bytes of |output| with cryptographically strong random
// data.
void RandBytes(void *output, size_t len);

// Fills a string of |len| with cryptographically strong random
// data and returns it. |len| should be nonzero.
//
// Note that this is a variation of |RandBytes| with a different return type.
std::string RandBytesAsString(size_t len);

#endif /* CPT_RAND_UTIL_H */