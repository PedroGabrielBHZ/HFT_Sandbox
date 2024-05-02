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
            boost::asio::ip::tcp::socket socket(io_service);
            socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("192.168.0.28"), 1234));

            for (;;)
            {
                // Create a buffer to read the data into
                boost::array<char, 128> buf;
                boost::system::error_code error;

                // Read data from the socket
                size_t len = socket.read_some(boost::asio::buffer(buf), error);

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