/*
 * PlayerWindow.h
 *
 *  Created on: Nov 12, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_PLAYERWINDOW_H_
#define INCLUDE_VNX_GTK_PLAYERWINDOW_H_

#include <vnx/gtk/PlayerWindowBase.hxx>
#include <vnx/gtk/ObjectDisplay.h>

#include <vnx/Player.h>
#include <vnx/PlayerClient.hxx>


namespace vnx {
namespace gtk {

class PlayerWindow : public PlayerWindowBase {
public:
	PlayerWindow(const std::string& vnx_name_);

protected:
	void on_show() override;

	void on_hide() override;

	void handle(std::shared_ptr<const vnx::PlayerStatus> value) override;

	bool on_render();

	void open_files();

	void on_open();

	void on_topics();

	void on_info();

	void on_play();

	void on_pause();

	void on_stop();

	void on_seek_relative(double secs);

	void on_seek_absolute();

	void on_toggle_repeat();

	void on_toggle_sync_mode();

	void on_speed_select();

private:
	vnx::PlayerClient player_sync;
	vnx::Handle<vnx::Player> player;
	std::shared_ptr<const vnx::PlayerStatus> player_status;

	double ignore_seek = -1;
	int64_t disable_update = 0;

	Gtk::Grid main_grid;
	Gtk::HBox seek_box;
	Gtk::Toolbar main_palette;
	Gtk::HScale seek_scale;
	Gtk::ComboBoxText speed_select;
	Gtk::HSeparator top_separator;
	Gtk::HSeparator bottom_separator;

	ObjectDisplay info_display;

	struct {
		Gtk::ToolButton open;
		Gtk::ToolButton topics;
		Gtk::ToolButton info;
	} setup;

	struct {
		Gtk::SeparatorToolItem separator;
		Gtk::ToolButton play;
		Gtk::ToolButton pause;
		Gtk::ToolButton stop;
	} control;

	struct {
		Gtk::SeparatorToolItem separator;
		Gtk::ToolButton rewind_2;
		Gtk::ToolButton rewind_1;
		Gtk::ToolButton forward_1;
		Gtk::ToolButton forward_2;
	} seek;

	struct {
		Gtk::SeparatorToolItem separator;
		Gtk::ToggleToolButton repeat;
		Gtk::ToggleToolButton sync_mode;
	} options;

	struct {
		Gtk::Grid bar;
		Gtk::Label info;
		Gtk::VSeparator seperator_1;
		Gtk::Label speed;
		Gtk::VSeparator seperator_2;
		Gtk::Label bandwidth;
		Gtk::VSeparator seperator_3;
		Gtk::Label state;
	} status;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_PLAYERWINDOW_H_ */
