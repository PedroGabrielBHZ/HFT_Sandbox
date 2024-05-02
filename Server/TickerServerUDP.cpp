#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <boost/asio.hpp>
#include <mutex>

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0, 100.0);

float getAssetValue()
{
    return distribution(generator);
}

void handleConnections(boost::asio::io_service &io_service, boost::asio::ip::udp::socket &socket, float &price)
{
    for (;;)
    {
        // Convert the asset value to a string
        std::string message = "Asset value: " + std::to_string(price) + "\n";

        // Send the asset value to the client
        boost::system::error_code ignored_error;
        boost::asio::ip::udp::endpoint remote_endpoint;
        socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);

        // Sleep for some time
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void updateAssetPrice(float &assetPrice)
{
    for (;;)
    {
        // Generate a new price
        float currentPrice = getAssetValue();

        // Update the asset price
        assetPrice = currentPrice;

        // Print the price
        std::cout << "Price updated: " << assetPrice << std::endl;

        // Sleep for some time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    // Initialize the asset price
    float assetPrice = 0.0;

    try
    {
        // Create an io_service
        boost::asio::io_service io_service;

        // Create a UDP socket
        boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 1234));

        // Start the connection handling thread
        std::thread connectionThread(handleConnections, std::ref(io_service), std::ref(socket), std::ref(assetPrice));

        // Start the price generation thread
        std::thread priceThread(updateAssetPrice, std::ref(assetPrice));

        // Wait for the threads to finish
        connectionThread.join();
        priceThread.join();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}