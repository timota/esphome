/*
 * A set of helpers that make the integration of ESPHome Sprinkler
 * with the relays and the display easier and shorter.
 */
#pragma once
#include "esphome.h"

namespace esphome {

/*
 * Toggle valve - start valve if inactive or shutdown controller
 */
/*
void toggle(int valve, switch_::Switch *relay,
            sprinkler::Sprinkler *sprinkler) {
  if (!id(relay).state) {
    id(sprinkler).start_single_valve(valve - 1);
  } else {
    id(sprinkler).shutdown();
  }
}
*/

/*
 * Prevent starting single valve program when button state change was triggered
 * by the controller
 *
 * Useful when turning valve on by push of a button button,
 * that is also used to indicate that the valve is on.
 */

/*
void conditional_on(int valve, switch_::Switch *relay,
                    sprinkler::Sprinkler *sprinkler) {
  if (!id(relay).state) {
    id(sprinkler).start_single_valve(valve - 1);
  }
}
*/

/*
 * Prevent shutting down the conroller when button state change was triggered by
 * the controller
 *
 * Useful when turning valve off by push of a button button,
 * that is also used to indicate that the valve is on.
 */

/*
void conditional_off(switch_::Switch *relay, sprinkler::Sprinkler *sprinkler) {
  if (id(relay).state) {
    id(sprinkler).shutdown();
  }
}
*/

/*
 * Format remaining time of the curent valve as a string (minute:second)
 */

/*
std::string remaining_time(sprinkler::Sprinkler *sprinkler) {
  uint32_t remaining_time = id(sprinkler).time_remaining().value_or(0);
  if (remaining_time == 0) {
    return "";
  }
  char result[12];
  sprintf(result, "%02d:%02d", (int)(remaining_time / 60),
          (int)(remaining_time % 60));
  return result;
}
*/

/*
 * Get time_remaining. Used to store it as global variable.
 */
uint32_t get_pause_time_remaining(sprinkler::Sprinkler *sprinkler) {
  if (id(sprinkler).time_remaining().has_value()) {
    return id(sprinkler).time_remaining().value_or(0);
  }
  return 0;
}

/*
 * Get Valve status: On/Off/Paused
 */
std::string get_valve_status(int valve, sprinkler::Sprinkler *sprinkler) {
  if (id(sprinkler).active_valve().has_value() && id(sprinkler).active_valve() == (valve - 1)) {
    return "On";
  }
  if (id(sprinkler).paused_valve().has_value() && id(sprinkler).paused_valve() == (valve - 1)) {
    return "Pause";
  }
  return "Off";
}

/*
 * Get remaining time based on valve status.
 * On     - current remaining_time from running Ctrl
 * Off    - configured valve run duration 
 * Pause  - value stored in global variable when pause action called 
*/
std::string remaining_time3(int valve, sprinkler::Sprinkler *sprinkler) {
  uint32_t remaining_time = 0;
  std::string valve_status = get_valve_status(valve, sprinkler);

  if (valve_status == "On" ) {
    remaining_time = id(sprinkler).time_remaining().value_or(0);
  } else if (valve_status == "Off") {
    remaining_time = id(sprinkler).valve_run_duration(valve - 1);
  } else if (valve_status == "Pause") {
    remaining_time = id(pause_time_remaining);
  }

  char result[12];
  sprintf(result, "%02d:%02d", (int)(remaining_time / 60),
          (int)(remaining_time % 60));
  return result;
}

/*
 * Toggle between resume/pause states
 */
void pause_resume_toggle(globals::GlobalsComponent<std::string> *ctrl_status, globals::GlobalsComponent<std::string> *ctrl_status_prev, sprinkler::Sprinkler *sprinkler) {
  if ( id(sprinkler).paused_valve().has_value() ) {
    ESP_LOGD("manual---->", "Resume ctrl state");
    id(sprinkler).resume();
    // restore state before pause
    id(ctrl_status) = id(ctrl_status_prev);
  } else {
    ESP_LOGD("manual---->", "Pause ctrl state");
    id(sprinkler).pause();
    // save current ctrl state as prev
    id(ctrl_status_prev) = id(ctrl_status);
    // set ctrl state to pause
    id(ctrl_status) = "Pause";
  }
}

void valve_toggle_with_global(int valve, globals::GlobalsComponent<std::string> *ctrl_status, 
                                  sprinkler::Sprinkler *sprinkler) {
  if (! id(id(sprinkler).valve_switch(valve - 1)).state()) {
    ESP_LOGD("manual---->", "Start Zone %i", valve);
    id(sprinkler).start_single_valve(valve - 1);
    id(ctrl_status) = "Zone";
  } else {
    ESP_LOGD("manual---->", "Shutdown controller");
    id(sprinkler).shutdown();
  }
}

}  // namespace esphome
