// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "Scheduler.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <future>



// Function to monitor user input
void monitorUserInput(TASK_SCHEDULER::Scheduler* scheduler) {
    std::string input;
    while (true) {
        std::cout << "Type 'exit' to stop the scheduler: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            if(scheduler->IsRunning())
            {
                scheduler->stop();
            }
            break;
        }
        std::cout << "Unknown command: " << input << std::endl;
    }
    std::cout << "Exit command typed " << input << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string configFilePath = argv[1];
    std::cout << "config file = "<< configFilePath << std::endl;
    try {
        boost::asio::io_context ioContext;
        
        // Pass config file path to the scheduler
        TASK_SCHEDULER::Scheduler scheduler(ioContext, configFilePath);

        auto fut = std::async(std::launch::async, monitorUserInput, &scheduler);

        // Start the Scheduler
        std::cout << "Starting Scheduler..." << std::endl;
        if(!scheduler.IsRunning())
        {
            scheduler.run();
            fut.wait();
            std::cout<<"after exiting user input monitor"<<std::endl;
        }
        else{
            throw std::runtime_error("Scheduler is already running");
        }
        
        
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
