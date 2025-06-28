#include <iostream>
#include <sstream>
#include <atomic>
#include "app.h"
#include "pose_runner.h"

void populate_monitors(Gtk::ComboBoxText* combo){
    auto display = Gdk::Display::get_default();
    if(display){
        int n_monitors = display->get_n_monitors();
        for(int i=0;i<n_monitors;i++){
            auto monitor = display->get_monitor(i);
            Gdk::Rectangle geometry;
            monitor->get_geometry(geometry);
            auto name = monitor->get_model();

            std::stringstream ss;
            ss << "Monitor " << i+1 << ": " << geometry.get_width()
                << "x" << geometry.get_height() << " at ("
                << geometry.get_x() << "," << geometry.get_y() << ")";
            combo->append(ss.str());
        }
        combo->set_active(0);
    }
    else {
        combo->append("No monitors found");
        combo->set_active(0);
    }
}

void handle_pressed(Gtk::Button* start_button, Gtk::ComboBoxText* combo, Gtk::TextView* output_box, bool& isRunning,
					std::shared_ptr<std::atomic_bool> stopFlag){
    //disable the dropdown, and change the button text and set a flag?
    if(!isRunning){
        start_button->set_label("Stop Detection");
        combo->set_sensitive(false);
        auto buffer = output_box->get_buffer();
        buffer->insert(buffer->end(), "Detection started...\n");
		//activate the thread
		stopFlag->store(false);
		run_pose_on_monitor(combo->get_active_row_number(), output_box, stopFlag);
    }
    else {
        start_button->set_label("Start Detection");
        combo->set_sensitive(true);
        auto buffer = output_box->get_buffer();
        buffer->insert(buffer->end(), "Detection stopped...\n");
    }
    isRunning = !isRunning;
}

Gtk::Box* build_ui() {
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, 5);
    auto combo = Gtk::make_managed<Gtk::ComboBoxText>();
    vbox->pack_start(*combo, Gtk::PACK_SHRINK);
    //get list of monitor lists
    populate_monitors(combo);

    auto button = Gtk::make_managed<Gtk::Button>("Start Detection");
    vbox->pack_start(*button, Gtk::PACK_SHRINK);


    //auto scroll and so it dosent overflow
    auto scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    //the acutal textbox for output
    auto output_box = Gtk::make_managed<Gtk::TextView>();
    output_box->set_editable(false);
    scroll->add(*output_box);
    vbox->pack_start(*scroll, Gtk::PACK_EXPAND_WIDGET);

    //flag for button and handle button clicked(lambda)
    //shared pointer is safer(using a regular bool gave me seg faults)
    std::shared_ptr<bool> isRunning = std::make_shared<bool>(false);
	//i guess this is also a shared pointer, flag for our thread
	std::shared_ptr<std::atomic_bool> stopFlag = std::make_shared<std::atomic_bool>(false);
    button->signal_clicked().connect([button, combo, output_box, isRunning, stopFlag] {
            handle_pressed(button, combo, output_box, *isRunning, stopFlag);
        });

    return vbox;
}

