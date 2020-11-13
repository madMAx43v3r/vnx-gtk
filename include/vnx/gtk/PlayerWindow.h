/*
 * PlayerWindow.h
 *
 *  Created on: Nov 12, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_PLAYERWINDOW_H_
#define INCLUDE_VNX_GTK_PLAYERWINDOW_H_

#include <vnx/gtk/WindowModule.h>
#include <vnx/gtk/ObjectDisplay.h>
#include <vnx/gtk/Util.h>
#include <vnx/gtk/PlayerWindowBase.hxx>

#include <vnx/Player.h>
#include <vnx/PlayerClient.hxx>

#include <gtkmm.h>


namespace vnx {
namespace gtk {

class PlayerWindow : public WindowModule<PlayerWindowBase> {
public:
	PlayerWindow(const std::string& vnx_name_)
		:	WindowModule(vnx_name_),
			player_sync(player_name),
			info_display(*this, "Record Info")
	{
		set_title("VNX Player | localhost:4444 | /tmp/vnxplayer.sock");
		set_default_size(width, height);
		set_icon_name("media-playback-start");

		// --- main grid ---
		{
			int row = 0;
			main_grid.attach(main_palette,			0, row++, 1, 1);
			main_grid.attach(top_separator,			0, row++, 1, 1);
			main_grid.attach(seek_box,				0, row++, 1, 1);
			main_grid.attach(bottom_separator,		0, row++, 1, 1);
			main_grid.attach(status.bar,			0, row++, 1, 1);
		}
		add(main_grid);

		// --- setup ---
		setup.open.set_icon_name("document-open");
		setup.topics.set_icon_name("drive-multidisk");
		setup.info.set_icon_name("dialog-information");

		setup.open.set_tooltip_text("Open");
		setup.topics.set_tooltip_text("Topics");
		setup.info.set_tooltip_text("Info");

		setup.topics.set_sensitive(false);
		setup.info.set_sensitive(false);

		setup.open.signal_clicked().connect(std::bind(&PlayerWindow::on_open, this));
		setup.topics.signal_clicked().connect(std::bind(&PlayerWindow::on_topics, this));
		setup.info.signal_clicked().connect(std::bind(&PlayerWindow::on_info, this));

		main_palette.append(setup.open);
		main_palette.append(setup.topics);
		main_palette.append(setup.info);

		// --- control ---
		control.play.set_icon_name("media-playback-start");
		control.pause.set_icon_name("media-playback-pause");
		control.stop.set_icon_name("media-playback-stop");

		control.play.set_tooltip_text("Play");
		control.pause.set_tooltip_text("Pause");
		control.stop.set_tooltip_text("Stop");

		control.play.set_sensitive(false);
		control.pause.set_sensitive(false);
		control.stop.set_sensitive(false);

		control.play.signal_clicked().connect(std::bind(&PlayerWindow::on_play, this));
		control.pause.signal_clicked().connect(std::bind(&PlayerWindow::on_pause, this));
		control.stop.signal_clicked().connect(std::bind(&PlayerWindow::on_stop, this));

		main_palette.append(control.separator);
		main_palette.append(control.play);
		main_palette.append(control.pause);
		main_palette.append(control.stop);

		// --- seek ---
		seek.rewind_2.set_icon_name("media-skip-backward");
		seek.rewind_1.set_icon_name("media-seek-backward");
		seek.forward_1.set_icon_name("media-seek-forward");
		seek.forward_2.set_icon_name("media-skip-forward");

		seek.rewind_2.set_tooltip_text("Rewind 1 min");
		seek.rewind_1.set_tooltip_text("Rewind 10 sec");
		seek.forward_1.set_tooltip_text("Forward 10 sec");
		seek.forward_2.set_tooltip_text("Forward 1 min");

		seek.rewind_2.set_sensitive(false);
		seek.rewind_1.set_sensitive(false);
		seek.forward_1.set_sensitive(false);
		seek.forward_2.set_sensitive(false);

		seek.rewind_2.signal_clicked().connect(std::bind(&PlayerWindow::on_seek_relative, this, -60));
		seek.rewind_1.signal_clicked().connect(std::bind(&PlayerWindow::on_seek_relative, this, -10));
		seek.forward_1.signal_clicked().connect(std::bind(&PlayerWindow::on_seek_relative, this, 10));
		seek.forward_2.signal_clicked().connect(std::bind(&PlayerWindow::on_seek_relative, this, 60));

		main_palette.append(seek.separator);
		main_palette.append(seek.rewind_2);
		main_palette.append(seek.rewind_1);
		main_palette.append(seek.forward_1);
		main_palette.append(seek.forward_2);

		// --- options ---
		options.repeat.set_icon_name("media-playlist-repeat");
		options.sync_mode.set_icon_name("changes-prevent");

		options.repeat.set_tooltip_text("Repeat");
		options.sync_mode.set_tooltip_text("Sync Mode");

		options.repeat.set_active(is_repeat);
		options.sync_mode.set_active(is_blocking);

		options.repeat.signal_toggled().connect(std::bind(&PlayerWindow::on_toggle_repeat, this));
		options.sync_mode.signal_toggled().connect(std::bind(&PlayerWindow::on_toggle_sync_mode, this));

		main_palette.append(options.separator);
		main_palette.append(options.repeat);
		main_palette.append(options.sync_mode);

		// --- scale ---
		seek_scale.set_digits(1);
		seek_scale.set_range(0, 100);
		seek_scale.set_increments(1, 10);
		seek_scale.set_has_origin(false);
		seek_scale.set_hexpand(true);
		seek_scale.signal_value_changed().connect(std::bind(&PlayerWindow::on_seek_absolute, this));

		// --- speed ---
		speed_select.append("1", "1x");
		speed_select.append("2", "2x");
		speed_select.append("3", "3x");
		speed_select.append("5", "5x");
		speed_select.append("10", "10x");
		speed_select.append("100", "100x");
		speed_select.append("0.5", "1/2");
		speed_select.append("0.33", "1/3");
		speed_select.append("0.2", "1/5");
		speed_select.append("0.1", "1/10");
		speed_select.append("0.01", "1/100");

		if(play_speed == 1) {
			speed_select.set_active_id("1");
		} else {
			speed_select.set_active(-1);
		}
		speed_select.signal_changed().connect(std::bind(&PlayerWindow::on_speed_select, this));

		seek_box.pack_start(seek_scale, true, true, 5);
		seek_box.pack_start(speed_select, false, true, 5);

		// --- status bar ---
		bottom_separator.set_margin_top(2);
		status.info.set_text("Open file(s) first.");
		status.info.set_hexpand();
		status.info.set_halign(Gtk::ALIGN_START);
		status.speed.set_text("?");
		status.bandwidth.set_text("?");
		status.state.set_text("?");
		{
			int i = 0;
			status.bar.attach(status.info, i++, 0, 1, 1);
			status.bar.attach(status.seperator_1, i++, 0, 1, 1);
			status.bar.attach(status.speed, i++, 0, 1, 1);
			status.bar.attach(status.seperator_2, i++, 0, 1, 1);
			status.bar.attach(status.bandwidth, i++, 0, 1, 1);
			status.bar.attach(status.seperator_3, i++, 0, 1, 1);
			status.bar.attach(status.state, i++, 0, 1, 1);
			status.bar.set_name("status_bar");
			status.bar.set_border_width(3);
			status.bar.set_column_spacing(3);
		}

		// --- info display ---
		info_display.set_size_request(600, 500);
		info_display.set_modal(true);

		Glib::signal_timeout().connect(sigc::mem_fun(*this, &PlayerWindow::on_render), 200, Glib::PRIORITY_LOW);

		show_all_children();
	}

protected:
	void init() override
	{
		vnx_virtual_time = false;
	}

	void main() override
	{
		subscribe(topic_status);

		Super::main();
	}

	void handle(std::shared_ptr<const vnx::PlayerStatus> value) override
	{
		player_status = value;
	}

	bool on_render()
	{
		const double percent = int(1000 * player_status->progress) / 10.;
		{
			const auto delta = vnx::get_wall_time_millis() - disable_update;
			if(delta < 0 || delta > 1000) {
				ignore_seek = percent;
				seek_scale.set_value(percent);
			}
		}
		status.info.set_text(get_date_string_ex("%Y-%m-%d %H:%M:%S %Z", false, player_status->current_time / 1000000) + " | "
				+ get_date_string_ex("%H:%M:%S", true, (player_status->end_time - player_status->begin_time) / 1000000) + " (HH:MM:SS) | "
				+ vnx::to_string(player_status->info.num_bytes / 1024 / 1024) + " MB");
		status.speed.set_text(vnx::to_string(play_speed) + "x (" + vnx::to_string(int(player_status->actual_speed * 100) / 100.) + "x)");
		status.bandwidth.set_text(vnx::to_string(int(player_status->bandwidth * 100) / 100.) + " MB/s");

		if(player_status->control.is_paused) {
			control.play.set_sensitive(true);
			control.pause.set_sensitive(false);
			status.state.set_text("PAUSED");
		} else {
			control.play.set_sensitive(false);
			control.pause.set_sensitive(true);
			status.state.set_text("PLAYING");
		}
		setup.topics.set_sensitive(true);
		setup.info.set_sensitive(true);
		return true;
	}

	void open_files()
	{
		player = new vnx::Player(player_name);
		player->files = files;
		player->play_speed = play_speed;
		player->is_repeat = is_repeat;
		player->is_blocking = is_blocking;
		player.start();

		bool is_good = false;
		try {
			auto info = player_sync.get_info();
			info_display.set_data(info.to_object());
			is_good = true;
		}
		catch(...) {
			status.info.set_text("Failed to open record file(s)!");
			info_display.set_data(vnx::Object());
		}
		control.stop.set_sensitive(is_good);
		seek.rewind_2.set_sensitive(is_good);
		seek.rewind_1.set_sensitive(is_good);
		seek.forward_1.set_sensitive(is_good);
		seek.forward_2.set_sensitive(is_good);
	}

	void on_vnx_load() override
	{
		if(!files.empty()) {
			open_files();
			if(is_autostart) {
				on_play();
			}
		}
	}

	void on_hide() override
	{
		info_display.hide();
	}

	void on_open()
	{
		Gtk::FileChooserDialog dialog(*this, "Open File(s) ...", Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
		dialog.add_button("Open", Gtk::RESPONSE_ACCEPT);
		dialog.set_filter(get_file_filter_dat());
		dialog.set_select_multiple(true);
		if(dialog.run() != Gtk::RESPONSE_ACCEPT) {
			return;
		}
		files = dialog.get_filenames();
		open_files();
	}

	void on_topics()
	{
		// TODO
	}

	void on_info()
	{
		info_display.run();
		info_display.hide();
	}

	void on_play()
	{
		player_sync.play_async();
	}

	void on_pause()
	{
		player_sync.pause_async();
	}

	void on_stop()
	{
		player_sync.stop_async();
	}

	void on_seek_relative(double secs)
	{
		player_sync.seek_by_time_async(secs * 1e6);
	}

	void on_seek_absolute()
	{
		const auto pos = seek_scale.get_value();
		if(ignore_seek < 0 || fabs(pos - ignore_seek) >= 1)
		{
			log(INFO) << "Seeking to position " << pos << " %";
			player_sync.seek_to_position_async(pos / 100.);
			disable_update = vnx::get_wall_time_millis();
		}
		ignore_seek = -1;
	}

	void on_toggle_repeat()
	{
		is_repeat = options.repeat.get_active();
		player_sync.vnx_set_config_async("is_repeat", vnx::Variant(is_repeat));
	}

	void on_toggle_sync_mode()
	{
		is_blocking = options.sync_mode.get_active();
		player_sync.vnx_set_config_async("is_blocking", vnx::Variant(is_blocking));
	}

	void on_speed_select()
	{
		vnx::from_string(speed_select.get_active_id(), play_speed);
		if(play_speed <= 0) {
			play_speed = 1;
			speed_select.set_active_id("1");
		}
		player_sync.set_speed_async(play_speed);
	}

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
