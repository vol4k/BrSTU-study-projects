#pragma once

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <cstdlib>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>

namespace SPI {
	using boost::asio::ip::tcp;
	using boost::asio::io_service;
	typedef boost::shared_ptr<tcp::socket> socket_ptr;
	typedef uint16_t PORT;
	typedef std::string IP;
	typedef enum {init, successed, failed} STATE;

	struct DATA {
		std::vector<std::string> send;
		std::vector<std::string> received;
	};

	class Server {
	public:
		Server(const PORT&);

		void PushMessage(std::string);
		std::string PullMessage();
		STATE State();
		bool SocketListIsEmpty();
	private:
		void Session();
		void ReadSession();
		void WriteSession();
	private:
		PORT m_port;
		DATA m_data;
		STATE m_state;
		boost::mutex m_mutex;
		std::vector<std::shared_ptr<io_service>> m_io_service;
		std::vector<std::shared_ptr<tcp::acceptor>> m_acceptor;
		std::vector<socket_ptr> m_socket;
	};

	class Client {
	public:
		Client(const IP&, const PORT&);

		void PushMessage(std::string);
		std::string PullMessage();
		STATE State();
	private:
		void Session();
		void ReadSession();
		void WriteSession();
	private:
		socket_ptr m_socket;
		IP m_ip;
		PORT m_port;
		DATA m_data;
		STATE m_state;
		boost::mutex m_mutex;
	};
}