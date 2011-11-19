/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "../../src/endpoint.hpp"
#include "../../src/roles/server.hpp"
#include "../../src/sockets/ssl.hpp"

#include <cstring>

typedef websocketpp::endpoint<websocketpp::role::server,websocketpp::socket::plain> plain_endpoint_type;
typedef websocketpp::endpoint<websocketpp::role::server,websocketpp::socket::ssl> ssl_endpoint_type;
typedef websocketpp::role::server<plain_endpoint_type>::handler_ptr plain_handler_ptr;
typedef websocketpp::role::server<ssl_endpoint_type>::handler_ptr ssl_handler_ptr;

template <typename T>
class echo_server_handler : public T::handler {
public:
	typedef typename websocketpp::role::server<T>::handler handler_type;
	typedef typename websocketpp::role::server<T>::handler_ptr handler_ptr;
	typedef typename handler_type::foo connection_ptr;
	
	void validate(connection_ptr connection) {
		//std::cout << "state: " << connection->get_state() << std::endl;
	}
	
	void on_open(connection_ptr connection) {
		//std::cout << "connection opened" << std::endl;
	}
	
	void on_close(connection_ptr connection) {
		//std::cout << "connection closed" << std::endl;
	}
	
	void on_message(connection_ptr connection,websocketpp::utf8_string_ptr msg) {
		//std::cout << "got message: " << *msg << std::endl;
		connection->send(*msg);
	}
	void on_message(connection_ptr connection,websocketpp::binary_string_ptr data) {
		//std::cout << "got binary message of length: " << data->size() << std::endl;
		connection->send(*data);
	}
	
	void http(connection_ptr connection) {
		connection->set_body("HTTP Response!!");
	}
	
	void on_fail(connection_ptr connection) {
		std::cout << "connection failed" << std::endl;
	}
};

int main(int argc, char* argv[]) {
	unsigned short port = 9002;
	bool ssl = false;
		
	if (argc == 2) {
		// TODO: input validation?
		port = atoi(argv[1]);
	}
	
	if (argc == 3) {
		// TODO: input validation?
		port = atoi(argv[1]);
		ssl = !strcmp(argv[2],"-ssl");
	}
	
	try {
		if (!ssl) {
			plain_handler_ptr h(new echo_server_handler<plain_endpoint_type>());
			plain_endpoint_type e(h);
			
			e.alog().set_level(websocketpp::log::alevel::CONNECT);
			e.alog().set_level(websocketpp::log::alevel::DISCONNECT);
			
			// TODO: fix
			//e.alog().set_level(websocketpp::log::alevel::CONNECT & websocketpp::log::alevel::DISCONNECT);
			e.elog().set_levels(websocketpp::log::elevel::ERROR,websocketpp::log::elevel::FATAL);
			
			std::cout << "Starting WebSocket echo server on port " << port << std::endl;
			e.listen(port);
		} else {
			ssl_handler_ptr h(new echo_server_handler<ssl_endpoint_type>());
			ssl_endpoint_type e(h);
			
			e.alog().set_level(websocketpp::log::alevel::CONNECT & websocketpp::log::alevel::DISCONNECT);
			e.elog().set_levels(websocketpp::log::elevel::ERROR,websocketpp::log::elevel::FATAL);
			
			std::cout << "Starting Secure WebSocket echo server on port " << port << std::endl;
			e.listen(port);
		}
		
		
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	
	return 0;
}
