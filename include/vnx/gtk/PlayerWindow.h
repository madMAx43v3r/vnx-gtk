/*
 * PlayerWindow.h
 *
 *  Created on: Nov 12, 2020
 *      Author: mad
 */

#ifndef INCLUDE_VNX_GTK_PLAYERWINDOW_H_
#define INCLUDE_VNX_GTK_PLAYERWINDOW_H_

#include <vnx/gtk/WindowModule.h>
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
			player_sync("Player")
	{
	}

protected:
	void main() override
	{
		Super::main();
	}

	void handle(std::shared_ptr<const vnx::RecorderStatus> value) override
	{
		//
	}

private:
	vnx::PlayerClient player_sync;
	vnx::Handle<vnx::Player> player;

};


} // gtk
} // vnx

#endif /* INCLUDE_VNX_GTK_PLAYERWINDOW_H_ */
