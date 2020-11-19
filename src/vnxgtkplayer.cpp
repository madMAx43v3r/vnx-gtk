/*
 * vnxgtkplayer.cpp
 *
 *  Created on: Nov 12, 2020
 *      Author: mad
 */

#include <vnx/gtk/PlayerWindow.h>

#include <vnx/vnx.h>
#include <vnx/Server.h>
#include <vnx/TcpEndpoint.hxx>
#include <vnx/UnixEndpoint.hxx>
#include <vnx/Terminal.h>


int main(int argc, char** argv)
{
	std::map<std::string, std::string> options;
	options["f"] = "files";
	options["x"] = "speed";
	options["b"] = "blocking";
	options["r"] = "repeat";
	options["p"] = "port";
	options["u"] = "unix-path";
	options["files"] = "file ...";
	options["port"] = "port";
	options["unix-path"] = "unix path";
	options["seek"] = "0-1";

	vnx::init("vnxgtkplayer", argc, argv, options);

	{
		vnx::Handle<vnx::Terminal> module = new vnx::Terminal("Terminal");
		module.start_detached();
	}
	{
		std::shared_ptr<vnx::TcpEndpoint> endpoint = vnx::clone(vnx::TcpEndpoint::create("0.0.0.0", 4444));
		vnx::read_config("port", endpoint->port);

		vnx::Handle<vnx::Server> server = new vnx::Server("TcpServer", endpoint);
		server->export_list.push_back(vnx::time_control);
		server.start_detached();
	}
	{
		std::shared_ptr<vnx::UnixEndpoint> endpoint = vnx::clone(vnx::UnixEndpoint::create("/tmp/vnxplayer.sock"));
		vnx::read_config("unix-path", endpoint->domain_path);

		vnx::Handle<vnx::Server> server = new vnx::Server("UnixServer", endpoint);
		server->export_list.push_back(vnx::time_control);
		server.start_detached();
	}

	auto app = Gtk::Application::create("vnx.gtk.player");

	auto module = new vnx::gtk::PlayerWindow("PlayerWindow");
	vnx::read_config("files", module->files);
	vnx::read_config("repeat", module->is_repeat);
	vnx::read_config("blocking", module->is_blocking);
	vnx::read_config("speed", module->play_speed);

	const int ret = app->run(*module);
	delete module;
	vnx::close();
	return ret;
}
