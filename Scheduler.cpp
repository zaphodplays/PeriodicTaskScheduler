// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "Scheduler.hpp"
#include <fstream>
#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

namespace TASK_SCHEDULER {

Scheduler::Scheduler(boost::asio::io_context& ioContext, const std::string& configFilePath)
    : ioContext(ioContext), workGuard(boost::asio::make_work_guard(ioContext)), isRunning(false) {

    // Read the config file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Error opening config file: " + configFilePath);
    }
    
    bool stateSaved = false;

    // Parse JSON config
    nlohmann::json configJson;
    configFile >> configJson;

    if(!configJson.contains(CONFIG))
    {
        throw std::runtime_error("Invalid config file: Missing 'config' root.");
    }

    nlohmann::json stateJson;
    auto& config = configJson[CONFIG];

    if(config.contains(THREAD_POOL_CAPACITY) && !config[THREAD_POOL_CAPACITY].is_null() )
    {
        num_threads = config[THREAD_POOL_CAPACITY];
    }
    else
    {
        //just using a default value if none specified
        num_threads = 3;
    }


    findState(config, stateSaved, stateJson);
    processTasks(stateJson, config, ioContext, stateSaved);

    
}

void Scheduler::findState(nlohmann::json &config, bool &stateSaved, nlohmann::json &stateJson)
{
    if (config.contains(STATEFILE))
    {
        stateFilePath = config[STATEFILE].is_null() ? "" : config[STATEFILE];
        if (!stateFilePath.empty())
        {
            std::cout << "state file is " << stateFilePath << std::endl;
            // Read the state file
            std::ifstream stateFile(stateFilePath);
            if (!stateFile.is_open())
            {
                std::cout << "state file does not exist yet" << std::endl;
            }
            else
            {
                std::cout << "found saved state file" << std::endl;
                stateSaved = true;
                // Parse JSON state

                stateFile >> stateJson;
            }
        }
    }
    else
    {
        std::cout << "state file config entry not defined" << std::endl;
    }
}

void Scheduler::processTasks(nlohmann::json &stateJson, nlohmann::json &config, boost::asio::io_context &ioContext, bool stateSaved)
{
    nlohmann::json tasksJson = stateJson[TASKS];
    // Add tasks to the scheduler based on config
    for (const auto &taskConfig : config[TASKS])
    {

        std::string name = taskConfig["name"];
        std::string type = taskConfig["type"];
        int period = taskConfig["period"];
        auto params = taskConfig["params"].get<std::map<std::string, std::string>>();

        auto task = Task::createTask(type, name, period, params, ioContext);
        if (task)
        {
            if (stateSaved && !tasksJson.is_null() && tasksJson.contains(task->getName()))
            {
                std::cout << "loading state for task " << task->getName() << std::endl;
                task->loadTaskStateFromJson(tasksJson[task->getName()]);
            }
            std::cout << "Before adding: " << task.use_count() << std::endl; 
            addTask(task);
            std::cout << "after adding: " << task.use_count() << std::endl; 
            scheduleTask(task);
            std::cout << "after scheduling: " << task.use_count() << std::endl; 
        }
    }
}

Scheduler::~Scheduler() {
    //cleaning up
    ioContext.stop();
    for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
}

void Scheduler::addTask(std::shared_ptr<Task> task) {
    tasks.push_back(std::move(task));
}

void Scheduler::scheduleTask(std::shared_ptr<Task> task) {
    auto timer = std::make_unique<boost::asio::steady_timer>(ioContext);
    taskTimers[task->getId()] = std::move(timer);
    startTaskExecution(task);
}

void Scheduler::run() {

    if(isRunning.exchange(true))
    {
        throw std::runtime_error("This Scheduler instance is already running\n");
    }

    std::cout << "Scheduler running with " << num_threads << " threads" << std::endl;

    // Launch threads to process io_context
    for (int i = 0; i < num_threads; ++i) {
            std::thread& aThread = threadPool.emplace_back([this]() {
            ioContext.run();  // Each thread processes the io_context event loop
        });
    }
    
}

void Scheduler::executeTask(std::shared_ptr<Task> task) {
    try {
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "Task ID " << task->getId() << " is being executed on thread " << this_id << std::endl;
        task->executeTask();
    } catch (const std::exception& ex) {
        std::cerr << "Task ID " << task->getId() << " failed: " << ex.what() << std::endl;
    }
}

void Scheduler::startTaskExecution(std::shared_ptr<Task> task) {
    auto& timer = taskTimers[task->getId()];
    int period = task->getPeriod();
    if(task->getRemainingTime() > 0 )
    {
        period = task->getRemainingTime();
    }
    timer->expires_after(std::chrono::milliseconds(period));
    task->setRemainingTime(-1);
    
    timer->async_wait([this, task](const boost::system::error_code& ec) {
        handleTaskExecution(task, ec);
    });
}

void Scheduler::handleTaskExecution(std::shared_ptr<Task> task, const boost::system::error_code& ec) {
    if (ec) {
        if (ec != boost::asio::error::operation_aborted) {
            std::cerr << "Error in task execution: " << ec.message() << std::endl;
        }
        return;
    }

    // Post the task execution to the io_context
    boost::asio::post(ioContext, [task]() {
        task->executeTask();
        std::cout << "Task executed: " << task->getName()
                  << " on thread " << std::this_thread::get_id() << std::endl;
    });

    // Reschedule the task
    startTaskExecution(task);
}

void Scheduler::stop()
{
    if(!isRunning.exchange(false))
    {
        throw std::runtime_error("This Scheduler instance is already stopped\n");
    }

    std::cout<<"Scheduler stop triggered"<<std::endl;
    workGuard.reset();
    ioContext.stop();
    for (auto& thread : threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    // Cancel timers and store remaining delays
    ProcessTaskTimers();

    std::cout << "Scheduler stopped." << std::endl;

    saveState();
}

void Scheduler::ProcessTaskTimers()
{
    for (auto &task : tasks)
    {
        // Retrieve the remaining time until the next scheduled execution
        auto &timer = taskTimers[task->getId()];
        auto now = std::chrono::steady_clock::now();
        auto expiry = timer->expiry();

        if (expiry > now)
        {
            auto remainingTime = std::chrono::duration_cast<std::chrono::milliseconds>(expiry - now);
            task->setRemainingTime(remainingTime.count());
        }
        else
        {
            // Task execution was due but not run yet; reset delay to full period
            task->setRemainingTime(-1);
        }
        timer->cancel();
    }
}

void Scheduler::saveState() {
    if(stateFilePath.empty())
    {
        std::cout<<"state file location unspecified, exiting without saving state"<<std::endl;
        return;
    }

    nlohmann::json state;
    nlohmann::json tasksJson;
    for (auto& task : tasks) {
        tasksJson[task->getName()] = task->getTaskStateAsJson();
    }
    state[TASKS] = tasksJson;
    std::ofstream outFile(stateFilePath);
    outFile << state.dump(1, '\t', true); 
    std::cout << "STATE SAVED TO FILE " << stateFilePath << std::endl;
}

bool Scheduler::IsRunning() const  {
    return isRunning.load();
}

} // namespace TASK_SCHEDULER
