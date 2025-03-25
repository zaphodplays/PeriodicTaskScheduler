// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "SystemTask.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

namespace TASK_SCHEDULER {

SystemTask::SystemTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext)
    : Task(name, period, ioContext) {
    type = TYPE;
}

void SystemTask::executeTask() {
    // Simulate a random blocking I/O operation (e.g., file read)
    std::cout << "Task ID " << getId() << " is performing a random I/O operation..." << std::endl;
    
    // Random sleep for 2 to 6 seconds
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2, 6);  // Sleep between 2 to 6 seconds
    
    int sleepDuration = dis(gen);
    std::cout << "Task ID " << getId() << " will sleep for " << sleepDuration << " seconds." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(sleepDuration));  // Simulate I/O blocking with random delay
    
    std::cout << "Task ID " << getId() << " finished random I/O operation on thread " 
            << std::this_thread::get_id() << std::endl;
    std::cout<<"-------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
}

const std::string SystemTask::getType() const {
    return type;
}

// Factory function for creating NetworkTask
std::shared_ptr<Task> SystemTask::create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext) {
    return std::make_shared<SystemTask>(name, period, params, ioContext);
}

// Registration function for NetworkTask
void SystemTask::registerFactory() {
    Task::registerTask(TYPE, &SystemTask::create);
}

// Static member to ensure registration at program startup
const bool SystemTask::isRegistered = []() {
    SystemTask::registerFactory();
    return true;
}();

} // namespace TASK_SCHEDULER

