#include "event_manager.h"
#include "game_event_manager_interface.h"

// How to process events:
void ProcessEvents() {
        EventStreamReader reader;
        void* event_data = nullptr;
        uint32_t event_type = 1;
        size_t data_size = 0;
        while (reader.GetNextEvent(event_type, data_size, event_data)) {

            switch (event_type) {
                // Todo: Is this right?
                case (uint32_t)HotKeyEvent::GB_EVENT_HOTKEY_TURN_OFF_LENS:
                {
                    // Do something with event_data....
                    break;
                }
                case GB_EVENT_NULL: //(optional..)
                    {
                        // No event generated this frame...
                        break;
                    }
                default:
                {
                    // No event generated this frame...
                    break;
                }
            }
        }
};
