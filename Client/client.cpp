#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <vector>
#include <fstream>
#include <iomanip>
#include <thread>
#include <sstream>
#include <string>

using boost::asio::ip::udp;

struct Address
{
    std::string ip_address;
    int port;
};

std::vector<Address> readCSV(const std::string &filename)
{
    std::vector<Address> addresses;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        Address addr;

        // Splitting the line by comma
        if (std::getline(ss, token, ','))
            addr.ip_address = token;
        if (std::getline(ss, token, ','))
            addr.port = std::stoi(token);
        std::cout << addr.ip_address << " " << addr.port << std::endl;

        addresses.push_back(addr);
    }
    return addresses;
}

float ping_exchange(std::string ipaddr, int port, boost::asio::io_context &io_context)
{
    boost::asio::ip::udp::endpoint e(boost::asio::ip::address::from_string(ipaddr), port);

    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::array<char, 1> send_buf = {{0}};
    socket.send_to(boost::asio::buffer(send_buf), e);

    std::array<char, 128> recv_buf;
    udp::endpoint s1e;
    size_t len = socket.receive_from(
        boost::asio::buffer(recv_buf), s1e);

    float price = std::stof(recv_buf.data());
    return price;
}

double arbitrage(std::vector<float> prices)
{
    float maxPrice(-INFINITY);
    float minPrice(INFINITY);

    int minPriceExchange, maxPriceExchange;

    int idx(0);
    for (float p : prices)
    {
        if (p > maxPrice)
        {
            maxPrice = p;
            maxPriceExchange = idx;
        }
        if (p < minPrice)
        {
            minPrice = p;
            minPriceExchange = idx;
        }
        idx++;
    }

    std::cout << std::fixed << std::setprecision(3) << "B$ " << minPrice << " @ " << minPriceExchange
              << "\tS$ " << maxPrice << " @ " << maxPriceExchange << "\t";

    return fabs((maxPrice - minPrice));
}

void traderJoe(boost::asio::io_context &io_context, std::vector<Address> exchanges)
{

    int txIdx(0);
    float profit(0);
    for (;;)
    {
        // Asset prices
        std::vector<float> prices;

        int eIdx(0);
        for (Address e : exchanges)
        {
            // std::cout << "Price at exchange " << eIdx++ << " $";
            prices.push_back(ping_exchange(e.ip_address, e.port, io_context));
        }

        float txProfit = arbitrage(prices);
        profit += txProfit;
        std::cout << std::fixed << std::setprecision(3) << "tx" << txIdx++ << "\t\tP$"
                  << txProfit << "\t\tTP$" << profit << std::endl;

        // Pause for one second before making the next connection
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Jog\n";
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <addresses.txt>" << std::endl;
            return 1;
        }

        // Fetch each exchange address
        std::vector<Address> exchanges = readCSV(argv[1]);

        // Initialize IO context
        boost::asio::io_context io_context;

        // Start the trading thread
        traderJoe(io_context, exchanges);

        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
