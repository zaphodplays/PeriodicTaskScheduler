// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once

#include "Task.hpp"
#include <boost/asio.hpp>
#include <string>

namespace TASK_SCHEDULER {

class NetworkTask : public Task {

/**
 * @brief base Network Task class
 * 
 */

public:
    NetworkTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);

    void executeTask() override;

    const std::string getType() const override;

protected:
   
    std::string domain;
    


private:
    static constexpr const char* TYPE = "network";

    // Self-registration logic
    static void registerFactory();
    static std::shared_ptr<Task> create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);
    static const bool isRegistered;
};

} // namespace TASK_SCHEDULER


