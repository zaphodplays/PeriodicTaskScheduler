// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once
#include "Task.hpp"
#include <string>

namespace TASK_SCHEDULER {

class SystemTask : public Task {

/**
 * @brief base Task class for system type of Tasks, 
 * 
 */
    
public:
    SystemTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);
    

    void executeTask() override;

    const std::string getType() const override;

private:

    static constexpr const char* TYPE = "system";

    // Self-registration logic
    static void registerFactory();
    static std::shared_ptr<Task> create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);
    static const bool isRegistered;
};

} // namespace TASK_SCHEDULER

