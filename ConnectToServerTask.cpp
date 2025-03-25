// Author: Amar Shukla email: amaresh.shukla@gmail.com
#include "ConnectToServerTask.hpp"
#include <iostream>
#include <boost/asio/ip/tcp.hpp>

namespace TASK_SCHEDULER {

    ConnectToServerTask::ConnectToServerTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext)
    : NetworkTask(name, period, params, ioContext), p_resolver(ioContext), p_socket(ioContext, boost::asio::ip::tcp::v4())
    {
        type = TYPE;

        metrics[METRIC_CONNECT_TIME_MS] = std::make_shared<Metric>(METRIC_CONNECT_TIME_MS);

        if(params.find("domain") != params.end())
        {
            domain = params.find("domain")->second;
        }
        if(params.find("port") != params.end())
        {
            port = params.find("port")->second;
        }
    }

    void ConnectToServerTask::executeTask()
    {
            startConnect();
    }

    const std::string ConnectToServerTask::getType() const {
        return type;
    }

    void ConnectToServerTask::startConnect() {
     

        using namespace boost::asio;

        ip::tcp::resolver::query query(domain, port);
        auto start_time = std::chrono::steady_clock::now();

        p_resolver.async_resolve(query,
            [this, start_time](const boost::system::error_code& ec, ip::tcp::resolver::iterator endpoint) {
                if (!ec) {
                    connectToServer(endpoint, start_time);
                } else {
                    std::cerr << "Error resolving domain " << domain << ": " << ec.message() << std::endl;
                    std::cout<<"-------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
                }
            });   

    }

    void ConnectToServerTask::connectToServer(boost::asio::ip::tcp::resolver::iterator endpoint, const std::chrono::steady_clock::time_point& start_time) {
        
        boost::asio::async_connect(p_socket, endpoint,
            [this, start_time](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator) {
                if (!ec) 
                {
                    auto end_time = std::chrono::steady_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                    if(metrics.find(METRIC_CONNECT_TIME_MS) != metrics.end())
                    {
                        std::shared_ptr<Metric> pingMetric = metrics.find(METRIC_CONNECT_TIME_MS)->second;
                        pingMetric->updateValue(duration.count());
                        std::cout<<"ConnectToServerTask :: domain: "<< domain<< " port : "<<port<<pingMetric->toString() << std::endl;
                    }
            
                    std::cout<<"-------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
                     
                } 
                else 
                {
                    std::cerr << "Error connecting to " << domain << ": " << ec.message() << std::endl;
                    std::cout<<"-------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
                }

                p_socket.close(); 
            });
    }



// Factory function for creating PingTask
std::shared_ptr<Task> ConnectToServerTask::create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext) {
    return std::make_shared<ConnectToServerTask>(name, period, params, ioContext);
}

// Registration function for PingTask
void ConnectToServerTask::registerFactory() {
    Task::registerTask(TYPE, &ConnectToServerTask::create);
}

// Static member to ensure registration at program startup
const bool ConnectToServerTask::isRegistered = []() {
    ConnectToServerTask::registerFactory();
    return true;
}();

} // end namespace TASK_SCHEDULER
