#include "main.h"
#include "Network/server.h"
#include "global_frame.h"

wxIMPLEMENT_APP(main);

main::main()
	: thread_pool_(4) {}

main::~main()
{
}

bool main::OnInit()
{
	frame* f = new frame();
	f->Show();
	SET_GLOBAL_FRAME(f);
	return true;
}

int main::OnRun()
{
	// TODO -
	// Command buttons with functionality, exit etc.
	// Clear Log/Output Log commands.
	// use operator overrides for the UI log output calling similar to stringstream.

	boost::asio::io_context io_context;

	std::list<server> servers;

	tcp::endpoint endpoint(tcp::v4(), 7077);
	servers.emplace_back(io_context, endpoint);

	boost::asio::post(thread_pool_,
		[&io_context]() {
			io_context.run();
		}
	);

	boost::asio::post(thread_pool_,
		[&io_context]() {
		io_context.run();
	}
	);

	boost::asio::post(thread_pool_,
		[&io_context]() {
		io_context.run();
	}
	);

	boost::asio::post(thread_pool_,
		[&io_context]() {
		io_context.run();
	}
	);

	// Run the wxApp default loop.
	wxApp::OnRun();

	io_context.stop();
	thread_pool_.join();

	return 0;
}
