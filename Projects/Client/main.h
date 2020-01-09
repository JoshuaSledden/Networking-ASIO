#pragma once
#include <array>
#include <future>
#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/use_future.hpp>
#include "frame.h"

class main : public wxApp
{
public:
	main();
	~main();

public:
	virtual bool OnInit();
	virtual int OnRun();

private:
	std::thread t;
};

