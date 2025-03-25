// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "Task.hpp"
#include <stdexcept>
#include <iostream>

namespace TASK_SCHEDULER {

int Task::taskIdCounter = 0;

Task::Task(const std::string& name, int period, boost::asio::io_context& ioContext)
    : id(generateUniqueId()), name(name), period(period), ioContext(ioContext) {

    }

int Task::generateUniqueId() {
    return taskIdCounter++;
}

int Task::getId() const {
    return id;
}


int Task::getPeriod() const {
    return period;
}

int Task::getRemainingTime() const  {
    return remaining_time;
}

void Task::setRemainingTime(int remaining)   {
    remaining_time = remaining;
}

std::string Task::getName() const {
    return name;
}

nlohmann::json Task::getMetricsAsJson() const {
    nlohmann::json metricsJson;
    for (const auto& [name, metric] : metrics) {
        metricsJson[name] = metric->toJson();
    }
    return metricsJson;
}

void Task::loadMetricsFromJson(const nlohmann::json& json) {
    for (const auto& [name, metricJson] : json.items()) {
        metrics[name]->fromJson(metricJson);
    }
}

void Task::loadTaskStateFromJson(const nlohmann::json& jsonTask)  {
    if(jsonTask.contains(NAME) && jsonTask[NAME] == getName())
    {
        //for now just override the task metrics with json metrics, 
        //later we might also check other stuff such as whether the task params such as period might have changed from the past
        if(jsonTask.contains(METRICS) && !jsonTask[METRICS].is_null())
        {
            loadMetricsFromJson(jsonTask[METRICS]);
        }
        if(jsonTask.contains(REMAINING_TIME) && !jsonTask[REMAINING_TIME].is_null())
        {
            setRemainingTime(jsonTask[REMAINING_TIME]);
        }
        
    }
}

nlohmann::json Task::getTaskStateAsJson() const {
    nlohmann::json taskState;
    taskState[REMAINING_TIME] = getRemainingTime();
    taskState[METRICS] = getMetricsAsJson();  // Assuming task has a getMetrics method
    return taskState;
}



//------------------------------------------------------------------------------------------------------------------------------------------//
//...These are static factory functions to register each subclass of Task in the static factory map.........................................//
// Static map to store factory functions
std::map<std::string, Task::TaskFactory>& Task::getFactoryMap() {
    static std::map<std::string, TaskFactory> factoryMap;
    return factoryMap;
}

// Factory method to create tasks
std::shared_ptr<Task> Task::createTask(const std::string& type, 
                                    const std::string& name, int period, 
                                    const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext) {
    
    
    // Ensure the task type is registered
    ensureTaskRegistration(type);

    auto it = getFactoryMap().find(type);
    if (it != getFactoryMap().end()) {
        return it->second(name, period, params, ioContext);
    }

    // Should not reach here due to ensureTaskRegistration, but added for safety
    throw std::runtime_error("Failed to create task. Unknown task type" + type);
}

// Method to register tasks
void Task::registerTask(const std::string& key, TaskFactory factory) {
    getFactoryMap()[key] = factory;
    std::cout<<"Registering TaskFactory for key = "<<key<<std::endl;
}

// Ensure the task type is registered
void Task::ensureTaskRegistration(const std::string& key) {
    // Check if the key exists in the map
    if (!key.empty() && getFactoryMap().find(key) == getFactoryMap().end()) {
        std::cerr << "Error: Task type '" << key << "' is not registered. Did you forget to include static registration in the Task subclass?" << std::endl;
        assert(false && "Unregistered task type. Check subclass registration logic.");
    }
}



} // namespace TASK_SCHEDULER
