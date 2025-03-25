// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "Metric.hpp"

namespace TASK_SCHEDULER
{

    void Metric::updateValue(double value) {
        std::lock_guard<std::mutex> lock(mutex);

        val = value;
        count++;

        // Update min and max
        if (val < min || count == 1) {
            min = val;
        }

        if (val > max || count == 1) {
            max = val;
        }

        // Update average incrementally
        avg = avg + (val - avg) / count;
    }

    double Metric::getValue() const {
        std::lock_guard<std::mutex> lock(mutex);
        return val;
    }

    double Metric::getMin() const {
        std::lock_guard<std::mutex> lock(mutex);
        return min;
    }

    double Metric::getMax() const {
        std::lock_guard<std::mutex> lock(mutex);
        return max;
    }

    double Metric::getAverage() const {
        std::lock_guard<std::mutex> lock(mutex);
        return avg;
    }

    size_t Metric::getCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return count;
    }

    const std::string& Metric::getName() const {
        return name;
    }

    const std::string Metric::toString() const
    {
        return "name = " + name + " , " + CURRENT + " = " + std::to_string(val) + " , " + MIN + " = " + std::to_string(min) + " , " + MAX + " = " + std::to_string(max) + " , " + AVERAGE + " = " + std::to_string(avg); 
    }

    nlohmann::json Metric::toJson() const {
        std::lock_guard<std::mutex> lock(mutex);
        return {
            {CURRENT, val},
            {MIN, min},
            {MAX, max},
            {AVERAGE, avg},
            {COUNT, count}
    };
}

    void Metric::fromJson(const nlohmann::json& json) {
        std::lock_guard<std::mutex> lock(mutex);
        val = json.at(CURRENT).get<double>();
        min = json.at(MIN).get<double>();
        max = json.at(MAX).get<double>();
        avg = json.at(AVERAGE).get<double>();
        count = json.at(COUNT).get<size_t>();
    }

}