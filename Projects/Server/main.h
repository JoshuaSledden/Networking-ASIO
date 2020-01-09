#pragma once
#include <array>
#include <future>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
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
	boost::asio::thread_pool thread_pool_;
};

