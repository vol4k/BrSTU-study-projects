#include "SocketProtocolInterface.hpp"

//////////////////////////////////////////////////////////////////////
////////////////////////////SERVER////////////////////////////////////
//////////////////////////////////////////////////////////////////////

SPI::Server::Server(const PORT& port)
{
	m_port = port;
	m_state = init;

	std::cout << "Сервер запущен на " << m_port << "-м порту" << std::endl;
	
	m_mutex.lock();	// locking for initialisation
	
	boost::thread t(boost::bind(&Server::Session, this));
}

void SPI::Server::Session()
{
	try {
		m_state = successed;
		
		m_mutex.unlock();
		while (true)
		{
			m_io_service.push_back(std::shared_ptr<io_service>(new io_service()));
			m_acceptor.push_back(
				std::move(std::shared_ptr<tcp::acceptor>
				(new tcp::acceptor(*m_io_service.back(), tcp::endpoint(tcp::v4(), m_port)))
				));
			m_socket.push_back(move(socket_ptr(new tcp::socket(*m_io_service.back()))));
			m_acceptor.back()->accept(*m_socket.back());
			if(m_socket.back()->is_open())
				std::cout << "Клиент подключен\n";
		}
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		m_state = failed;
	}
}

void SPI::Server::ReadSession()
{
	size_t index = 0;
	try {
		for (auto& socket : m_socket)
		{
			if (!socket->is_open())
			{
				index++;
				continue;
			}
			std::string data;
			data.resize(1024);
			auto buffer = boost::asio::buffer(&data[0], data.size());
			boost::asio::read(*socket, buffer);
			m_data.received.push_back(std::move(data));
			index++;
		}
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		m_socket.erase(m_socket.begin() + index);
		m_io_service.erase(m_io_service.begin() + index);
		m_acceptor.erase(m_acceptor.begin() + index);
	}
}

void SPI::Server::WriteSession()
{
	size_t index = 0;
	try {
		for (auto& socket : m_socket)
		{
			if (!socket->is_open()) 
			{
				index++;
				continue;
			}
			for (auto& Message : m_data.send)
			{
				Message.resize(1024);
				auto buffer = boost::asio::buffer(&Message[0], Message.size());
				boost::asio::write(*socket, buffer);
			}
			index++;
		}
		m_data.send.clear();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		m_socket.erase(m_socket.begin() + index);
		m_io_service.erase(m_io_service.begin() + index);
		m_acceptor.erase(m_acceptor.begin() + index);
	}
}

void SPI::Server::PushMessage(std::string message)
{
	m_data.send.push_back(std::move(message));
	WriteSession();
}

std::string SPI::Server::PullMessage()
{
	ReadSession();
	
	if (m_data.received.size())
	{
		while (m_data.received.front().empty())
			m_data.received.erase(m_data.received.begin());
	}

	if (m_data.received.empty())
		return "";
	else
		return std::move(m_data.received.front());
}

SPI::STATE SPI::Server::State()
{
	if (m_state == init)
	{
		m_mutex.lock();		//waiting for mutex free
		m_mutex.unlock();
	}
	return m_state;
}

bool SPI::Server::SocketListIsEmpty()
{
	return m_socket.empty();
}

//////////////////////////////////////////////////////////////////////
////////////////////////////CLIENT////////////////////////////////////
//////////////////////////////////////////////////////////////////////

SPI::Client::Client(const IP& ip, const PORT& port)
{
	m_ip = ip;
	m_port = port;
	m_state = init;

	std::cout << "Попытка подключиться к серверу " << m_ip << ':' << m_port << std::endl;
	m_mutex.lock();	// locking for initialisation
	boost::thread t(boost::bind(&Client::Session, this));
}

void SPI::Client::Session()
{
	try {
		static boost::asio::io_service io_service;
		static tcp::endpoint ep(boost::asio::ip::address::from_string(m_ip), m_port);

		m_socket = move(socket_ptr(new tcp::socket(io_service)));
		m_socket->connect(ep);

		if (m_socket->is_open())
		{
			std::cout << m_ip << ':' << m_port << " - подключение удалось" << std::endl;
			m_state = successed;
		}
		else
		{
			std::cout << m_ip << ':' << m_port << " - подключение не удалось" << std::endl;
			m_state = failed;
		}
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		m_state = failed;
	}
	m_mutex.unlock();
}

void SPI::Client::ReadSession()
{
	if (!m_socket) 
	{
		system("pause");
		exit(1);
	}
	try {
		if (!m_socket->is_open()) return;
		std::string data;
		data.resize(1024);
		auto buffer = boost::asio::buffer(&data[0], data.size());
		boost::asio::read(*m_socket, buffer);
		m_data.received.push_back(std::move(data));
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;	
		m_socket = nullptr;
	}
}

void SPI::Client::WriteSession()
{
	if (!m_socket)
	{
		system("pause");
		exit(1);
	}
	try {
		if (!m_socket->is_open()) return;
		for (auto& Message : m_data.send)
		{
			Message.resize(1024);
			auto buffer = boost::asio::buffer(&Message[0], Message.size());
			boost::asio::write(*m_socket, buffer);
		}
		m_data.send.clear();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		m_socket = nullptr;
	}
}

void SPI::Client::PushMessage(std::string message)
{
	m_data.send.push_back(std::move(message));
	WriteSession();
}

std::string SPI::Client::PullMessage()
{
	ReadSession();

	if (m_data.received.size())
	{
		while (m_data.received.front().empty())
			m_data.received.erase(m_data.received.begin());
	}

	if (m_data.received.empty())
		return "";
	else
		return std::move(m_data.received.front());
}

SPI::STATE SPI::Client::State()
{
	if (m_state == init) 
	{
		m_mutex.lock();		//waiting for mutex free
		m_mutex.unlock();
	}
	return m_state;
}