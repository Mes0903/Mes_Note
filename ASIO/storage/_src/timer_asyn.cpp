#include <boost/asio.hpp>
#include <iostream>

void print(const boost::system::error_code & /*e*/)
{
  std::cout << "Hello, world!" << std::endl;
}

int main()
{
  boost::asio::io_context io;

  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
  boost::asio::steady_timer t2(io, boost::asio::chrono::seconds(2));

  std::cout << "test1\n";
  t.async_wait(&print);
  t2.async_wait(&print);

  std::cout << "test2\n";
  io.run();

  std::cout << "test3\n";

  return 0;
}