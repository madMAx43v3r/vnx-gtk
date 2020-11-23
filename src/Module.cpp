/*
 * Module.cpp
 *
 *  Created on: Nov 19, 2020
 *      Author: mad
 */

#include <vnx/gtk/Module.h>


namespace vnx {
namespace gtk {

Module::Module(const std::string& name)
	:	ModuleBase(name)
{
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &Module::on_vnx_poll), vnx_poll_interval_ms, Glib::PRIORITY_LOW);
}

Module::~Module()
{
}

void Module::add_window(Gtk::Window& parent, bool is_transient, bool is_modal)
{
	if(is_modal || is_transient) {
		set_transient_for(parent);
	} else {
		if(auto application = parent.get_application()) {
			application->add_window(*this);
		}
	}
	set_modal(is_modal);
}

void Module::on_show()
{
	vnx_setup();
	on_vnx_poll();
	Gtk::Window::on_show();
}

void Module::on_hide()
{
	Gtk::Window::on_hide();
	vnx_cleanup();
}

void Module::vnx_stop()
{
	Gtk::Window::close();
}

void Module::vnx_restart()
{
	// we do nothing here
}

bool Module::on_vnx_poll()
{
	vnx_process(false);
	return true;
}


} // gtk
} // vnx
