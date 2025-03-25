// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "Task.hpp"


namespace TASK_SCHEDULER {

/**
 * @brief This class manages concurrent execution of multiple Tasks by employing the boost::asio::io_context to manage all async operations
 *  - Reads the tasks to be executed from a config file
 *  - Creates Task objects based on the config
 *  - Checks if there is a previous state for this created task and merges with the state to update metrics, remaining time etc
 *  - Creates and maintains a thread pool(capacity provided by config), these threads call io_context.run() to allow concurrent execution of tasks.
 *  - Associates a timer(managed by io_context) to this task,  so that the task's execution asynchronously waits on the timer expiration
 *  - Saves state for each task when stop is invoked 
 *  - Stops the scheduler by by using the io_context to gracefully cancelling all timers and saving the task states
 * 
 */

class Scheduler {

public:
    Scheduler(boost::asio::io_context &ioContext, const std::string &configFilePath);

    ~Scheduler();

    void run();

    void stop();

    bool IsRunning() const;
    
private:

    void addTask(std::shared_ptr<Task> task);

    //creates a timer for this task that asynchronously waits for its time-period
    void scheduleTask(std::shared_ptr<Task> task);

    void processTasks(nlohmann::json &stateJson, nlohmann::json &config, boost::asio::io_context &ioContext, bool stateSaved);

    void findState(nlohmann::json &config, bool &stateSaved, nlohmann::json &stateJson);

    void saveState();

    void ProcessTaskTimers();

    void executeTask(std::shared_ptr<Task> task);

    // starts the timer and associates the handler 
    void startTaskExecution(std::shared_ptr<Task> task);

    // Post the task execution to the io_context
    void handleTaskExecution(std::shared_ptr<Task> task, const boost::system::error_code& ec);

    boost::asio::io_context& ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;
    std::atomic<bool> isRunning;
    
    std::vector<std::shared_ptr<Task>> tasks;
    std::map<int, std::unique_ptr<boost::asio::steady_timer>> taskTimers;
    std::vector<std::thread> threadPool;
    int num_threads;
    std::string stateFilePath;

    static constexpr const char* CONFIG = "config";
    static constexpr const char* THREAD_POOL_CAPACITY = "thread_pool_capacity";
    static constexpr const char* STATEFILE = "stateFile";
    static constexpr const char* TASKS = "tasks";
};

} // namespace TASK_SCHEDULER
