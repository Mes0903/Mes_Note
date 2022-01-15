#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#define _WIN32_WINNT 0X0A00
#endif

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

std::vector<char> vBuffer(20 * 1024);

void getData(tcp::socket &socket)
{
  socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()),
                         [&](boost::system::error_code ec, std::size_t length) {
                           if (!ec) {
                             std::cout << "\n\nRead " << length << " bytes\n\n";

                             for (int i = 0; i < length; i++)
                               std::cout << vBuffer[i];

                             getData(socket);
                           }
                         });
}

int main()
{
  boost::system::error_code ec;
  boost::asio::io_context context;

  boost::asio::io_context::work idleWork(context);

  std::thread thrContext = std::thread([&]() { context.run(); });

  tcp::endpoint endpoint(boost::asio::ip::make_address("51.38.81.49", ec), 80);
  tcp::socket socket(context);

  socket.connect(endpoint, ec);

  if (!ec)
    std::cout << "Connect\n";
  else
    std::cout << "Failed to connect\n"
              << ec.message() << '\n';

  if (socket.is_open()) {
    getData(socket);

    std::string sRequest =
      "Get /index.html HTTP/1.1\r\n"
      "Host: example.com\r\n"
      "Connection: cloose\r\n\r\n";

    socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()), ec);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20000ms);

    context.stop();
    if (thrContext.joinable())
      thrContext.join();
  }

  return 0;
}