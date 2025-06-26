#include <iostream>
#include "app.h"

Gtk::Box* build_ui() {
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, 5);

    auto combo = Gtk::make_managed<Gtk::ComboBoxText>();
    combo->append("Monitor 1");
    combo->append("Monitor 2");
    vbox->pack_start(*combo, Gtk::PACK_SHRINK);

    auto button = Gtk::make_managed<Gtk::Button>("Start");
    vbox->pack_start(*button, Gtk::PACK_SHRINK);

    button->signal_clicked().connect([] {
        std::cout << "Button clicked!" << std::endl;
    });

    return vbox;
}

