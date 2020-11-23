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
	timer_slot = sigc::mem_fun(*this, &Module::on_vnx_timer);
	dispatcher.connect(sigc::mem_fun(*this, &Module::on_vnx_notify));
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
	on_vnx_notify();
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

void Module::notify(std::shared_ptr<Pipe> pipe)
{
	Super::notify(pipe);
	if(!is_notified.exchange(true)) {
		dispatcher.emit();
	}
}

void Module::on_vnx_notify()
{
	is_notified = false;
	const auto timeout_us = vnx_process(false);
	if(timeout_us >= 0) {
		Glib::signal_timeout().connect_once(timer_slot, timeout_us / 1000, Glib::PRIORITY_LOW);
	}
}

void Module::on_vnx_timer()
{
	on_vnx_notify();
}


} // gtk
} // vnx
