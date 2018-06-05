#include "manager.h"

static double g_K = 16.0;

template <typename T>
static int
sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace manager {

long long g_bot_ids;
double g_default_rating = 1200;

std::pair<double, double>
MM::operator()(double rating1, double rating2, double result1) const
{
    std::pair<double, double> ret = {rating1, rating2};
    double e1 = sgn(rating1 - rating2) * (0.5 - 1.0 / std::pow(2.0, std::abs(rating1 - rating2) / 100.0 + 1)) + 0.5;
    double e2 = 1.0 - e1;
    ret.first += g_K * (result1 - e1);
    ret.second += g_K * (1.0 - result1 - e2);
    return ret;
}

}