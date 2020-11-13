/*
 * Util.h
 *
 *  Created on: Nov 13, 2020
 *      Author: mad
 */

#ifndef VNX_GTK_INCLUDE_VNX_GTK_UTIL_H_
#define VNX_GTK_INCLUDE_VNX_GTK_UTIL_H_

#include <gtkmm.h>


namespace vnx {
namespace gtk {

inline
Glib::RefPtr<Gtk::FileFilter> get_file_filter_dat()
{
	auto filter = Gtk::FileFilter::create();
	filter->set_name("VNX Binary File (*.dat)");
	filter->add_pattern("*.dat");
	filter->add_mime_type("application/octet-stream");
	return filter;
}


} // gtk
} // vnx

#endif /* VNX_GTK_INCLUDE_VNX_GTK_UTIL_H_ */
