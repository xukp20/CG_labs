#pragma once
#include <random>

static std::mt19937 gen(0);
static std::uniform_real_distribution<float> dis(0, 1);

#define RAND_UNIFORM dis(gen)
#define RAND_UNIFORM_RANGE(a, b) (a + (b - a) * RAND_UNIFORM)
#define RAND_SIGNED (2.0 * RAND_UNIFORM - 1.0)
#define SMALL_POSI dis(gen) * 1e-6