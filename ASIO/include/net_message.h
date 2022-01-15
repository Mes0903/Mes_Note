#ifndef NET_MESSAGE
#define NET_MESSAGE
#include "net_message.h"

namespace net {

  template <typename T>
  struct message_header {
    T id{};
    uint32_t size = 0;
  };

  template <typename T>
  struct message {
    message_header<T> header{};
    std::vector<uint8_t> body;

    // return size of entire message packet in bytes.
    std::size_t size() const
    {
      return sizeof(message_header<T>) + body.size();
    }

    friend std::ostream &operator<<(std::ostream &os, const message<T> &msg)
    {
      os << "ID: " << static_cast<uint32_t>(msg.header.id) << " Size: " << msg.header.size;
      return os;
    }

    template <typename Datatype>
      friend message<T> & << operator<<(message<T> &msg, const Datatype &data)
    {
      // Check that the type of the data being pushed is trivially copyable.
      static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be push in message buffer");

      // Cache current size of vector, as this will be the point we insert the data.
      std::size_t __size = msg.body.size();

      // Resize the vector by the size of the data being pushed
      msg.body.resize(msg.body.size() + sizeof(DataType));

      // Physically copy the data into the newly allocated vector space
      std::memcpy(msg.body.data() + __size, &data, sizeof(DataType));

      // Recalculate the message size
      msg.header.size = msg.size();

      // Return the target message so it can be "chained"
      return msg;
    }
  };

}    // namespace net

#endif