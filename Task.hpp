// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once

#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <cassert>
#include "Metric.hpp"

namespace TASK_SCHEDULER {


/**
 * @brief This is the base class for tasks
 * Each subclass has a different 'type' attached to it to support a type based factory and dynamically resolve the type creation.
 * This is implemented by Task maintaining a static map of (string type, TaskFactory) to resolve the correct TaskFactory needed at runtime,
 * and create the correct subclass based on the provied type string
 */
class Task {
public:
    Task(const std::string& name, int period, boost::asio::io_context& ioContext);
    
    virtual ~Task() = default;

    virtual void executeTask() = 0;

    virtual const std::string getType() const = 0;

    int getId() const;

    int getPeriod() const;

    int getRemainingTime() const;

    void setRemainingTime(int remaining);

    std::string getName() const;

    const Metric& getMetric(std::string& name) const;

    nlohmann::json getTaskStateAsJson() const;

    void loadTaskStateFromJson(const nlohmann::json& jsonTask);

protected:

    nlohmann::json getMetricsAsJson() const;

    void loadMetricsFromJson(const nlohmann::json& jsonMetric);

protected:

    int id;

    std::string name;

    // time period in milliseconds
    int period;

    // time remaining in milliseconds till next execution
    int remaining_time;

    //a map to store metrics based on key name
    std::unordered_map<std::string, std::shared_ptr<Metric> > metrics;

    std::string type;

protected:

    // The io_context thats bound to this task. This can be needed in some tasks for their i/o operations
    boost::asio::io_context& ioContext;

public: 

    static constexpr const char* NAME = "name";
    static constexpr const char* TYPE = "type";
    static constexpr const char* PERIOD = "period";
    static constexpr const char* REMAINING_TIME = "remaining_time";
    static constexpr const char* METRICS = "metrics";


private:

    static int taskIdCounter;  // Simple integer

    static int generateUniqueId();

    using TaskFactory = std::shared_ptr<Task>(*)(const std::string&, int, const std::map<std::string, std::string>&, boost::asio::io_context& ioContext);

    static std::map<std::string, TaskFactory>& getFactoryMap();

    /*
    *   Below are the static methods that define a task factory so that different task objects will be returned by createTask based on the 'type' parameter which is read from the config file for each task config 
    */

protected:

    static void registerTask(const std::string& key, TaskFactory factory);

    // Error handling mechanism for unregistered tasks
    static void ensureTaskRegistration(const std::string& key);

public:

    // Factory method to create tasks
    static std::shared_ptr<Task> createTask(const std::string& type, const std::string& name, int period, 
                                            const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);

};

} // namespace TASK_SCHEDULER



