// Author: Amar Shukla email: amaresh.shukla@gmail.com
#pragma once
#include "NetworkTask.hpp"

namespace TASK_SCHEDULER {

    class ConnectToServerTask : public NetworkTask {

    /**
     * @brief To establish a connection to a remote server (e.g., google.com) on a specific port (e.g., 80 for HTTP).
                Metric Collected: connection time, i.e., the time taken to establish a successful TCP connection.
     * 
     */

    public :

        ConnectToServerTask(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);

        void executeTask() override;

        virtual const std::string getType() const override;

    private :

        void startConnect();

        void connectToServer(boost::asio::ip::tcp::resolver::iterator endpoint, const std::chrono::steady_clock::time_point& start_time);

        std::string domain;

        std::string port;

        boost::asio::ip::tcp::resolver p_resolver;
        boost::asio::ip::tcp::socket p_socket;
        boost::asio::ip::tcp::resolver::iterator p_endpoint;

    private:

    static constexpr const char* TYPE = "network:connect";
    static constexpr const char* METRIC_CONNECT_TIME_MS = "connect-time(ms)";

    // Self-registration logic
    static void registerFactory();
    static std::shared_ptr<Task> create(const std::string& name, int period, const std::map<std::string, std::string>& params, boost::asio::io_context& ioContext);
    static const bool isRegistered;

    };

}