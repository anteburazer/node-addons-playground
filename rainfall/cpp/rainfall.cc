#include <algorithm>
#include <cmath>
#include "rainfall.h" 

// TODO: What '&' means?
double calculate_avg_rainfall(location & loc) {
    double total = 0;

    // TODO: How for loop works and what is 'auto' type?
    for (const auto &sample : loc.samples) {
        total += sample.rainfall;
    }
    
    return total / loc.samples.size();
}

rain_result calculate_rainfall_statistics(location & loc) {
    rain_result result;
    double ss = 0;

    result.n = loc.samples.size();
    result.mean = calculate_avg_rainfall(loc);

    for (const auto &sample : loc.samples) {
        ss += pow(sample.rainfall - result.mean, 2);
    }
    result.standard_deviation = sqrt(ss/(result.n-1));

    // TODO: Compile error
    // std::sort(loc.samples.begin(), loc.samples.end());

    if (result.n %2 == 0) {
        result.median = (loc.samples[result.n / 2 - 1].rainfall + loc.samples[result.n / 2].rainfall) / 2;
    } else {
        result.median = loc.samples[result.n / 2].rainfall;
    }

    return result;
}