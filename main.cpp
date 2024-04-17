#include <halcyon/event.hpp>
#include <halcyon/video.hpp>

int main(int argc, char *argv[]) {
  hal::context ctx;

  hal::video::system vid{ctx};

  hal::video::window wnd{vid, "Ligma", {640, 480}};
  hal::event::handler eh{vid.events};

  while (true) {
    while (eh.poll()) {
      switch (eh.event_type()) {
        using enum hal::event::type;

      case quit_requested:
        return EXIT_SUCCESS;

      case key_pressed:
        if (eh.keyboard().button() == hal::keyboard::button::esc)
          return EXIT_SUCCESS;

      case window:
        if (eh.window().event_type() != hal::event::window::type::moved)
          HAL_PRINT("Received window event: ",
                    hal::to_string(eh.window().event_type()));

      default:
        break;
      }
    }
  }
}