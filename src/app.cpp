#include <iostream>
#include <sstream>
#include "app.h"

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

void handle_start(Gtk::Button* start_button, Gtk::ComboBoxText* combo, bool& isRunning){
    //disable the dropdown, and change the button text and set a flag?
    if(!isRunning){
        start_button->set_label("Stop Detection");
        combo->set_sensitive(false);
    }
    else {
        start_button->set_label("Start Detection");
        combo->set_sensitive(true);
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

    bool isRunning = false;
    button->signal_clicked().connect([=, &isRunning] {
        handle_start(button, combo, isRunning);
    });

    return vbox;
}

