#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

float ping_exchange(boost::asio::ip::udp::endpoint e, boost::asio::io_context &io_context)
{
    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::array<char, 1> send_buf = {{0}};
    socket.send_to(boost::asio::buffer(send_buf), e);

    std::array<char, 128> recv_buf;
    udp::endpoint s1e;
    size_t len = socket.receive_from(
        boost::asio::buffer(recv_buf), s1e);

    float price = std::stof(recv_buf.data());
    std::cout << price << std::endl;
    return price;
}

double arbitrage(float p1, float p2)
{
    return fabs(p1 - p2);
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 5)
        {
            std::cerr << "Usage: client <host1> <port> <host2> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        // Define the target endpoints
        boost::asio::ip::udp::endpoint e1(boost::asio::ip::address::from_string(argv[1]), std::stoi(argv[2]));
        boost::asio::ip::udp::endpoint e2(boost::asio::ip::address::from_string(argv[3]), std::stoi(argv[4]));

        float ex1p, ex2p, stonks(0), diff;
        for (int i = 0; i < 100; i++)
        {
            // Ping exchange 1
            ex1p = ping_exchange(e1, io_context);

            // Ping exchange 2
            ex2p = ping_exchange(e2, io_context);

            diff = arbitrage(ex1p, ex2p);
            std::cout << "Made $" << diff << " now.\n";

            stonks += diff;
            std::cout << "Current profit $" << stonks << std::endl;

            // Pause for one second before making the next connection
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
