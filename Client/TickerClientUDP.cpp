#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

int main()
{
    for (;;)
    {
        // Sleep for a second
        std::this_thread::sleep_for(std::chrono::seconds(1));

        try
        {
            // Create an I/O service
            boost::asio::io_service io_service;

            // Create a socket and connect to the server
            boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
            boost::asio::ip::udp::endpoint server_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234);

            std::cout << "Ready!\n";

            for (;;)
            {
                // Create a buffer to read the data into
                boost::array<char, 128> buf;
                boost::system::error_code error;

                // Read data from the socket
                boost::asio::ip::udp::endpoint sender_endpoint;
                size_t len = socket.receive_from(boost::asio::buffer(buf), sender_endpoint, 0, error);

                // Check if an error occurred
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer
                else if (error)
                    throw boost::system::system_error(error); // Some other error

                // Print the data
                std::cout.write(buf.data(), len);

                // Sleep for a second
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}