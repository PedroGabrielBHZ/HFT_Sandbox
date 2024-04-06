# Ticker Server and Client

This project contains a server (TickerServer) and a client (TickerClient) that communicate over TCP using the Boost.Asio library. The server generates a stream of asset prices and sends them to the client.

## Project Structure

````plaintext
Client/
    Dockerfile
    Makefile
    TickerClient
    TickerClient.cpp
Server/
    Dockerfile
    Makefile
    TickerServer
    TickerServer.cpp
README.md
````

## Building the Project

### Server

To build and run the server, navigate to the `Server/` directory and use the following commands:

```bash
docker build -t tickerserver -f Dockerfile .
docker run -p 1234:1234 -d tickerserver
```

This will build a Docker image for the server and run it in a new container. The server listens for connections on port 1234.

Client

To build and run the client, navigate to the `Client/` directory and use the following commands:

```bash
docker build -t tickerclient -f Dockerfile .
docker run -d tickerclient
```

This will build a Docker image for the client and run it in a new container. The client connects to the server at the IP address specified in `TickerClient.cpp`.

## Note

The client and server are designed to run on the same machine. If you want to run them on different machines, you need to update the IP address in `TickerClient.cpp` to the IP address of the machine running the server.

## License

This project is licensed under the MIT License.
