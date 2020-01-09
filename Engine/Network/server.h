#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "message.h"
#include "global_frame.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<message> message_queue;

//----------------------------------------------------------------------

class game_client
{
public:
	virtual ~game_client() {}
	virtual void deliver(const message& msg) = 0;
};

typedef std::shared_ptr<game_client> game_client_ptr;

//----------------------------------------------------------------------

class server_lobby
{
public:
	void join(game_client_ptr client) {
		game_clients_.insert(client);
		for (auto msg : recent_msgs_)
			client->deliver(msg);

		GLOBAL_FRAME_LOG("Client joined the lobby");
	}

	void leave(game_client_ptr client) {
		GLOBAL_FRAME_LOG("Client left the lobby");
		game_clients_.erase(client);
	}

	void deliver(const message& msg) {
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		for (auto client : game_clients_)
			client->deliver(msg);
	}

private:
	std::set<game_client_ptr> game_clients_;
	enum { max_recent_msgs = 100 };
	message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class server_session
	: public game_client,
	public std::enable_shared_from_this<server_session>
{
public:
	server_session(tcp::socket socket, server_lobby& lobby)
		: socket_(std::move(socket)), lobby_(lobby) {}

	void start() {
		GLOBAL_FRAME_LOG("Session Started");
		lobby_.join(shared_from_this());
		do_read_header();
	}

	void deliver(const message& msg) {
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
			do_write();
	}

private:
	void do_read_header() {
		auto self(shared_from_this());

		boost::asio::async_read(
			socket_,
			boost::asio::buffer(read_msg_.data(), message::header_length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec && read_msg_.decode_header())
					do_read_body();
				else
					lobby_.leave(shared_from_this());
			}
		);
	}

	void do_read_body() {
		auto self(shared_from_this());

		boost::asio::async_read(
			socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					std::string s(read_msg_.body(), read_msg_.body_length());
					GLOBAL_FRAME_LOG(s);

					lobby_.deliver(read_msg_);
					do_read_header();
				}
				else
					lobby_.leave(shared_from_this());
			}
		);
	}

	void do_write() {
		auto self(shared_from_this());

		boost::asio::async_write(
			socket_,
			boost::asio::buffer(write_msgs_.front().data(),
			write_msgs_.front().length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					write_msgs_.pop_front();
					if (!write_msgs_.empty())
						do_write();
				}
				else
					lobby_.leave(shared_from_this());
			}
		);
	}

	tcp::socket socket_;
	server_lobby& lobby_;
	message read_msg_;
	message_queue write_msgs_;
};

//----------------------------------------------------------------------

class server {
public:
	server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint)
		: acceptor_(io_context, endpoint) { 
		GLOBAL_FRAME_LOG("Server initialized.");
		// Starts the recursive server loop.
		do_accept();
	}

private:
	void do_accept() {
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket) {
				if (!ec)
					std::make_shared<server_session>(std::move(socket), lobby_)->start();

				do_accept();
			}
		);
	}

	tcp::acceptor acceptor_;
	server_lobby lobby_;
};