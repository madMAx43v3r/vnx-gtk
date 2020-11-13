/*
 * ObjectDisplay.h
 *
 *  Created on: Nov 13, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_OBJECTDISPLAY_H_
#define INCLUDE_VNX_GTK_OBJECTDISPLAY_H_

#include <vnx/gtk/ObjectView.h>


namespace vnx {
namespace gtk {

class ObjectDisplay : public Gtk::Dialog {
public:
	ObjectDisplay(Gtk::Window& parent, const std::string& title)
	{
		set_transient_for(parent);
		set_default_size(400, -1);
		set_title(title);

		view.set_hexpand(true);
		view.set_vexpand(true);
		area.add(view);

		auto content = get_content_area();
		content->pack_start(area, true, true);

		add_button("_Close", Gtk::RESPONSE_ACCEPT);
		set_default_response(Gtk::RESPONSE_ACCEPT);

		show_all_children();
	}

	void set_data(	const vnx::Object& object,
					const std::map<std::string, ObjectView::options_t>& options = std::map<std::string, ObjectView::options_t>())
	{
		view.set_data(object, options);
	}

private:
	ObjectView view;
	Gtk::ScrolledWindow area;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_OBJECTDISPLAY_H_ */
