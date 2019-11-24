#include <vector>
#include <string>
#include <uv.h>
#include <v8.h>

using namespace std;

class sample {
    public:
        // sample();
        // sample(string d, double r);
        string date;
        double rainfall;
};

class location {
    public: 
        double longitude;
        double latitude;
        vector<sample> samples;
};

class rain_result {
   public:
       float median;
       float mean;
       float standard_deviation;
       int n;
};

struct Work {
  uv_work_t  request;
  v8::Persistent<v8::Function> callback;

  std::vector<location> locations;
  std::vector<rain_result> results;
};

double calculate_avg_rainfall(location & loc);
rain_result calculate_rainfall_statistics(location & loc);