/*
 * ObjectView.h
 *
 *  Created on: Sep 25, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_OBJECTVIEW_H_
#define INCLUDE_VNX_GTK_OBJECTVIEW_H_

#include <vnx/Object.hpp>

#include <gtkmm.h>

#include <memory>
#include <functional>


namespace vnx {
namespace gtk {

class ObjectView : public Gtk::ListViewText {
public:
	struct options_t {
		bool hide = false;
		std::string unit;
		std::string key_name;
		std::function<std::string(const vnx::Variant&)> render;
		std::function<vnx::Variant(const vnx::Variant&)> transform;
	};

	bool auto_expand = false;

	ObjectView(	bool show_header = true,
				Gtk::SelectionMode selection_mode = Gtk::SELECTION_SINGLE,
				const std::string& field_col_name = "Field",
				const std::string& value_col_name = "Value")
		:	ListViewText(2, false, selection_mode)
	{
		record.add(col_key);
		record.add(col_value);
		list = Gtk::TreeStore::create(record);

		set_model(list);
		set_rules_hint(true);
		set_enable_tree_lines(true);
		set_headers_visible(show_header);
		set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_VERTICAL);
		set_tooltip_column(1);
		{
			auto* col = get_column(0);
			col->set_title(field_col_name);
			col->set_resizable(true);
		}
		{
			auto* col = get_column(1);
			col->set_title(value_col_name);
			col->set_max_width(150);
		}
	}

	void set_data(	const vnx::Object& object,
					const std::map<std::string, options_t>& options = std::map<std::string, options_t>())
	{
		list->clear();
		is_tree = false;
		append_object(object, list->children(), "", options);
		if(is_tree) {
			set_show_expanders(true);
			if(auto_expand) {
				expand_all();
			}
		} else {
			set_show_expanders(false);
		}
	}

private:
	void append_object(	const vnx::Object& object, const Gtk::TreeNodeChildren& node, const std::string& path,
						const std::map<std::string, options_t>& options)
	{
		for(const auto& entry : object.field)
		{
			std::string key = entry.first;
			vnx::Variant value = entry.second;
			if(key.substr(0, 2) == "__") {
				continue;
			}
			auto iter = options.find(path + entry.first);
			if(iter != options.end()) {
				const auto& opt = iter->second;
				if(opt.hide) {
					continue;
				}
				if(opt.transform) {
					value = opt.transform(value);
				}
			}
			if(value.is_array()) {
				std::vector<vnx::Variant> array;
				value.to(array);
				if(array.size() > 1) {
					vnx::Object tmp;
					int i = 0;
					for(const auto& entry : array) {
						tmp["[" + vnx::to_string(i++) + "]"] = entry;
					}
					value = tmp;
				}
			}
			if(iter != options.end()) {
				const auto& opt = iter->second;
				if(!opt.key_name.empty()) {
					key = opt.key_name;
				}
				if(opt.render) {
					value = opt.render(value);
				}
				if(!value.is_object() && !opt.unit.empty()) {
					value = value.to_string_value() + " " + opt.unit;
				}
			}
			auto row = list->append(node);
			row->set_value<Glib::ustring>(0, key);
			if(value.is_object()) {
				is_tree = true;
				append_object(value.to_object(), row->children(), path + entry.first + ".", options);
			} else {
				row->set_value<Glib::ustring>(1, value.to_string_value());
			}
		}
	}

private:
	bool is_tree = false;
	Glib::RefPtr<Gtk::TreeStore> list;

	Gtk::TreeModel::ColumnRecord record;
	Gtk::TreeModelColumn<Glib::ustring> col_key;
	Gtk::TreeModelColumn<Glib::ustring> col_value;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_OBJECTVIEW_H_ */
