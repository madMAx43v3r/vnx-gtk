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

	int64_t vnx_poll_interval_ms = 10;		///< Polling interval for VNX messages [ms]

protected:
	void on_show() override;

	void on_hide() override;

	void vnx_stop() override;

	void vnx_restart() override;

private:
	bool on_vnx_poll();

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_MODULE_H_ */
