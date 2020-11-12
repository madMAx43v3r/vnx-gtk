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

/** \brief Base class to implement a VNX GTK window.
 *
 */
template<typename TModule>
class WindowModule : public Gtk::Window, public TModule {
public:
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

		TModule::main();
	}

	void handle(std::shared_ptr<const vnx::Message> msg) override
	{
		if(auto task = std::dynamic_pointer_cast<const vnx::Task>(msg))
		{
			// handle tasks in VNX thread
			TModule::handle(task);
			return;
		}
		if(auto sample = std::dynamic_pointer_cast<const vnx::Sample>(msg))
		{
			if(sample->topic == vnx::shutdown) {
				Window::close();
				TModule::exit();
			}
		}
		{
			// send message to GTK thread
			std::unique_lock<std::mutex> lock(TModule::vnx_mutex);
			while(TModule::vnx_do_run() && next_msg) {
				msg_condition.wait(lock);
			}
			next_msg = msg;
		}
		msg_dispatcher.emit();
	}

	virtual void on_vnx_msg()
	{
		std::shared_ptr<const vnx::Message> msg;
		{
			std::lock_guard<std::mutex> lock(TModule::vnx_mutex);
			msg = next_msg;
			next_msg = nullptr;
		}
		msg_condition.notify_all();

		if(msg) {
			try {
				TModule::handle(msg);
			} catch(const std::exception& ex) {
				TModule::log(WARN) << ex.what();
			}
		}
	}

	virtual void on_vnx_load() {}

private:
	Glib::Dispatcher msg_dispatcher;
	Glib::Dispatcher load_dispatcher;
	std::condition_variable msg_condition;
	std::shared_ptr<const vnx::Message> next_msg;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_APPLICATIONWINDOWMODULE_H_ */
