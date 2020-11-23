/*
 * Module.h
 *
 *  Created on: Nov 19, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_MODULE_H_
#define INCLUDE_VNX_GTK_MODULE_H_

#include <vnx/ModuleBase.h>

#include <atomic>
#include <gtkmm.h>


namespace vnx {
namespace gtk {

class Module : public ModuleBase, public Gtk::Window {
public:
	/** \brief Creates a new Module with given name.
	 *
	 * @param name Module name (does not have to be unique)
	 */
	Module(const std::string& name);

	virtual ~Module();

	void add_window(Gtk::Window& parent, bool is_modal = false, bool is_transient = false);

protected:
	void on_show() override;

	void on_hide() override;

	void vnx_stop() override;

	void vnx_restart() override;

private:
	void notify(std::shared_ptr<Pipe> pipe) override;

	void on_vnx_notify();

	void on_vnx_timer();

private:
	sigc::slot<void> timer_slot;
	Glib::Dispatcher dispatcher;

	std::atomic_bool is_notified {false};

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_MODULE_H_ */
