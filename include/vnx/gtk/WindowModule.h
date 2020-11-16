/*
 * WindowModule.h
 *
 *  Created on: Nov 12, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_APPLICATIONWINDOWMODULE_H_
#define INCLUDE_VNX_GTK_APPLICATIONWINDOWMODULE_H_

#include <vnx/Module.h>

#include <gtkmm.h>


namespace vnx {
namespace gtk {

/** \brief Base class to implement a VNX GTK Window.
 *
 */
template<typename TModule>
class WindowModule : public Gtk::Window, public TModule {
public:
	typedef WindowModule Super;

	WindowModule(const std::string& vnx_name_)
		:	TModule(vnx_name_)
	{
		msg_dispatcher.connect(sigc::mem_fun(*this, &WindowModule::on_vnx_msg));
		load_dispatcher.connect(sigc::mem_fun(*this, &WindowModule::on_vnx_load));
	}

	virtual ~WindowModule() {}

protected:
	void main() override
	{
		load_dispatcher.emit();

		Module::main();
	}

	void handle(std::shared_ptr<const vnx::Message> msg) override
	{
		if(auto task = std::dynamic_pointer_cast<const vnx::Task>(msg))
		{
			// handle tasks in VNX thread
			Module::handle(task);
			return;
		}
		{
			// send message to GTK thread
			std::lock_guard<std::mutex> lock(msg_mutex);
			next_msg = msg;
		}
		msg_dispatcher.emit();	// notify GTK thread of new message
		{
			// wait for GTK thread to finish processing the message
			std::unique_lock<std::mutex> lock(msg_mutex);
			while(next_msg && Module::vnx_do_run()) {
				msg_condition.wait(lock);
			}
		}
	}

	virtual void on_vnx_msg()
	{
		std::shared_ptr<const vnx::Message> msg;
		{
			std::lock_guard<std::mutex> lock(msg_mutex);
			msg = next_msg;
		}
		if(msg) {
			try {
				Module::handle(msg);
			} catch(const std::exception& ex) {
				Module::log(Module::WARN) << ex.what();
			}
		}
		if(auto sample = std::dynamic_pointer_cast<const vnx::Sample>(msg)) {
			if(sample->topic == vnx::shutdown) {
				Gtk::Window::close();
			}
		}
		// notify VNX thread that we are finished processing the message
		{
			std::lock_guard<std::mutex> lock(msg_mutex);
			next_msg = nullptr;
		}
		msg_condition.notify_all();
	}

	virtual void on_vnx_load() {}

private:
	std::mutex msg_mutex;
	std::condition_variable msg_condition;
	std::shared_ptr<const vnx::Message> next_msg;

	Glib::Dispatcher msg_dispatcher;
	Glib::Dispatcher load_dispatcher;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_APPLICATIONWINDOWMODULE_H_ */
