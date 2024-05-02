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

void handleConnections(boost::asio::io_service &io_service, boost::asio::ip::tcp::acceptor &acceptor, float &price)
{
    for (;;)
    {
        // Create a socket for the connection
        boost::asio::ip::tcp::socket socket(io_service);

        // Wait for a connection
        acceptor.accept(socket);

        // Convert the asset value to a string
        std::string message = "Asset value: " + std::to_string(price) + "\n";

        // Send the asset value to the client
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);

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

        // Listen for new connections on port 1234
        boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234));

        // Start the connection handling thread
        std::thread connectionThread(handleConnections, std::ref(io_service), std::ref(acceptor), std::ref(assetPrice));

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