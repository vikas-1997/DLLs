// Libs.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Interface.h"

#include "Libs.h"
#include <iostream>

using namespace std;

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        /*
        if (argc != 4)
        {
            std::cerr <<
                "Usage: websocket-client-sync <host> <port> <text>\n" <<
                "Example:\n" <<
                "    websocket-client-sync echo.websocket.org 80 \"Hello, world!\"\n";
            return EXIT_FAILURE;
        }
        std::string host = argv[1];
        auto const  port = argv[2];
        auto const  text = argv[3];
        */
        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };
        
        cout << "starting connection" << endl;
        string host = "localhost";
        string port = "9999";
        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(ws.next_layer(), results);

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ':' + std::to_string(ep.port());

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string("11") +
                    " websocket-client-coro");
            }));

        // Perform the websocket handshake
        ws.handshake(host, "/");

        beast::flat_buffer buffer;

        // Read a message into our buffer
        while (true) {
            ws.read(buffer);
            string cmd = boost::beast::buffers_to_string(buffer.data());
            send_cmd(cmd);
            std::cout << beast::make_printable(buffer.data()) << std::endl;
            while (true)
            {
                if (avail())
                {
                    string s = get_result();
                    cout << "sending" << s << endl;
                    ws.write(net::buffer(s));
                    cout << "sent" << endl;
                    cout << s << endl;
                }
                if (!avail() && idle())
                {
                    break;
                }
            }
            buffer.consume(buffer.size());
            // Send the message
        }
        

        // This buffer will hold the incoming message
       

        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}