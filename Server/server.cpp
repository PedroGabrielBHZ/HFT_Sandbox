#include <array>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

std::default_random_engine generator(std::time(nullptr));
std::uniform_real_distribution<float> distribution(-0.001, 0.001);

// sort of global?
float t1_price = 25;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

float sampleDistribution()
{
    return distribution(generator);
}

void updateAssetPrice()
{
    for (;;)
    {
        // Sample deltas
        float delta1 = sampleDistribution();

        // Update the asset price
        t1_price += delta1;

        // Cap asset price at zero
        if (t1_price < 0.0)
        {
            t1_price = 0.0;
        }

        // Print the price
        std::cout << make_daytime_string() << "Price: $" << t1_price << std::endl;

        // Sleep for some time
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::string serializeTicker()
{
    using namespace std; // For time_t, time and ctime;

    // Fetch ticker price
    std::string price = std::to_string(t1_price);

    return price;
}

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context &io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket &socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = serializeTicker();

        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this()));
    }

private:
    tcp_connection(boost::asio::io_context &io_context)
        : socket_(io_context)
    {
    }

    void handle_write()
    {
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_context &io_context, int port)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        tcp_connection::pointer new_connection =
            tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&tcp_server::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code &error)
    {
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
};

class udp_server
{
public:
    udp_server(boost::asio::io_context &io_context, boost::asio::ip::port_type port)
        : port_(port), socket_(io_context, udp::endpoint(udp::v4(), port_))
    {
        std::cout << "Listening on port " << port_ << std::endl;
        start_receive();
    }

private:
    void start_receive()
    {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&udp_server::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code &error,
                        std::size_t /*bytes_transferred*/)
    {
        if (!error)
        {
            std::shared_ptr<std::string> message(
                new std::string(serializeTicker()));

            socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                                  boost::bind(&udp_server::handle_send, this, message,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(std::shared_ptr<std::string> /*message*/,
                     const boost::system::error_code & /*error*/,
                     std::size_t /*bytes_transferred*/)
    {
    }

    boost::asio::ip::port_type port_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main(int argc, char *argv[])
{
    // Start the price generation thread
    std::thread priceThread(updateAssetPrice);

    boost::asio::ip::port_type port = boost::lexical_cast<boost::asio::ip::port_type>(argv[1]);

    try
    {
        boost::asio::io_context io_context;
        // tcp_server server1(io_context, port);
        udp_server server2(io_context, port);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}