#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "message.h"

#include "global_frame.h"

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;

class client
{
public:
	client(boost::asio::io_context& io_context,
		const tcp::resolver::results_type& endpoints)
		: io_context_(io_context),
		socket_(io_context) {
		// connects to the server and starts the recursive client loop.
		do_connect(endpoints);
	}

	void write(const message& msg) {
		boost::asio::post(
			io_context_,
			[this, msg]() {
				bool write_in_progress = !write_msgs_.empty();
				write_msgs_.push_back(msg);
				if (!write_in_progress) {
					do_write();
				}
			}
		);
	}

	void close() {
		boost::asio::post(io_context_, [this]() { socket_.close(); });
	}

private:
	void do_connect(const tcp::resolver::results_type& endpoints) {
		GLOBAL_FRAME_LOG("Attempting to connect...");
		boost::asio::async_connect(
			socket_,
			endpoints,
			[this](boost::system::error_code ec, tcp::endpoint) {
				if (!ec) {
					do_read_header();
				}
			}
		);
	}

	void do_read_header() {
		boost::asio::async_read(
			socket_,
			boost::asio::buffer(read_msg_.data(), message::header_length),
			[this](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec && read_msg_.decode_header()) {
					do_read_body();
				}
				else {
					socket_.close();
				}
			}
		);
	}

	void do_read_body() {
		boost::asio::async_read(
			socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					std::string s(read_msg_.body(), read_msg_.body_length());
					GLOBAL_FRAME_LOG(s);

					do_read_header();
				}
				else {
					socket_.close();
				}
			}
		);
	}

	void do_write() {
		boost::asio::async_write(
			socket_,
			boost::asio::buffer(write_msgs_.front().data(),
			write_msgs_.front().length()),
			[this](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					write_msgs_.pop_front();
					if (!write_msgs_.empty()) {
						do_write();
					}
				}
				else {
					socket_.close();
				}
			}
		);
	}

private:
	boost::asio::io_context& io_context_;
	tcp::socket socket_;
	message read_msg_;
	message_queue write_msgs_;
};
