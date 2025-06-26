#include <gtkmm.h>
#include "app.h"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "com.example.bodylanguagec");

    Gtk::Window window;
    window.set_title("Body Language C");
    window.set_default_size(400, 300);

    auto content = build_ui();
    window.add(*content);

    window.show_all();

    return app->run(window);
}

