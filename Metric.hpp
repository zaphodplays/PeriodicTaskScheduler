// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

namespace TASK_SCHEDULER {

class Metric {

public:
    Metric(const std::string& name) : name(name), count(0), val(0), min(0), max(0), avg(0) {}

    void updateValue(double value);

    double getValue() const;

    double getMin() const;

    double getMax() const;

    double getAverage() const;

    size_t getCount() const;

    const std::string& getName() const;

    const std::string toString() const;

    nlohmann::json toJson() const;

    void fromJson(const nlohmann::json& json);


private:
    std::string name;         // Name of the metric
    double val;                // Most recent value
    double min;               // Minimum value
    double max;               // Maximum value
    double avg;               // Sum of all values (for calculating average)
    size_t count;             // Count of values added
    mutable std::mutex mutex; // Mutex for thread-safe access

public:
    static constexpr const char* CURRENT = "current";
    static constexpr const char* MIN = "min";
    static constexpr const char* MAX = "max";
    static constexpr const char* AVERAGE = "average";
    static constexpr const char* COUNT = "count";
};

} // namespace TASK_SCHEDULER

