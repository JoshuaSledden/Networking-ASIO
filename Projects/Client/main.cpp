#include "main.h"
#include "Network/client.h"
#include "global_frame.h"

wxIMPLEMENT_APP(main);

main::main()
{
}

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

	std::string host = "127.0.0.1";
	std::string port = "7077";

	boost::asio::io_context io_context;
	tcp::resolver resolver(io_context);
	auto endpoints = resolver.resolve(host, port);
	client c(io_context, endpoints);

	t = std::thread([&io_context]() { io_context.run(); });

	std::string s("Hello Server!");
	message msg;
	msg.body_length(std::strlen(s.c_str()));
	std::memcpy(msg.body(), s.c_str(), msg.body_length());
	msg.encode_header();
	c.write(msg);

	// Run the wxApp default loop.
	wxApp::OnRun();

	c.close();
	t.join();

	return 0;
}
