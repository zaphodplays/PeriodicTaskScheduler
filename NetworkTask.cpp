// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "NetworkTask.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

namespace TASK_SCHEDULER {

NetworkTask::NetworkTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext)
    : Task(name, period, ioContext) {
    type = TYPE;
}

void NetworkTask::executeTask() {
    // Simulate a random blocking delay 
    std::cout << "Task ID " << getId() << " is performing a random network operation..." << std::endl;
    
    // Random sleep for 1 to 5 seconds
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);  // Sleep between 1 to 5 seconds
    
    int sleepDuration = dis(gen);
    std::cout << "Task ID " << getId() << " will sleep for " << sleepDuration << " seconds." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(sleepDuration)); // Simulate random delay
    
    std::cout << "Task ID " << getId() << " finished random network operation on thread " 
                << std::this_thread::get_id() << std::endl;

    std::cout<<"-------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
}

const std::string NetworkTask::getType() const
{
    return type;
}

// Factory function for creating NetworkTask
std::shared_ptr<Task> NetworkTask::create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext) {
    return std::make_shared<NetworkTask>(name, period, params, ioContext);
}

// Registration function for NetworkTask
void NetworkTask::registerFactory() {
    Task::registerTask(TYPE, &NetworkTask::create);
}

// Static member to ensure registration at program startup
const bool NetworkTask::isRegistered = []() {
    NetworkTask::registerFactory();
    return true;
}();

} // namespace TASK_SCHEDULER
