/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <GCS_MAVLink/GCS.h>

class QuadPlane;
class AP_MotorsMulticopter;
// Transition empty base class
class Transition
{
public:

    Transition(QuadPlane& _quadplane, AP_MotorsMulticopter*& _motors):quadplane(_quadplane),motors(_motors) {};

    virtual void update() = 0;

    virtual void VTOL_update() = 0;

    virtual void force_transistion_complete() = 0;

    virtual bool complete() const = 0;

    virtual void restart() = 0;

    virtual uint8_t get_log_transision_state() const = 0;

    virtual bool active() const = 0;

    virtual bool show_vtol_view() const = 0;

    virtual void set_FW_roll_pitch(int32_t& nav_pitch_cd, int32_t& nav_roll_cd, bool& allow_stick_mixing) {};

    virtual bool set_FW_roll_limit(int32_t& roll_limit_cd) { return false; }

    virtual bool allow_update_throttle_mix() const { return true; }

    virtual bool update_yaw_target(float& yaw_target_cd) { return false; }

    virtual MAV_VTOL_STATE get_mav_vtol_state() const = 0;

protected:

    // refences for convenience
    QuadPlane& quadplane;
    AP_MotorsMulticopter*& motors;

};

// Transition for separate left thrust quadplanes
class SLT_Transition : public Transition
{
public:

    using Transition::Transition;

    void update() override;

    void VTOL_update() override;

    void force_transistion_complete() override {
        transition_state = TRANSITION_DONE; 
        transition_start_ms = 0;
        transition_low_airspeed_ms = 0;
    };

    bool complete() const override { return transition_state == TRANSITION_DONE; }

    void restart() override { transition_state = TRANSITION_AIRSPEED_WAIT; }

    uint8_t get_log_transision_state() const override { return static_cast<uint8_t>(transition_state); }

    bool active() const override;

    bool show_vtol_view() const override;

    bool set_FW_roll_limit(int32_t& roll_limit_cd) override;

    bool allow_update_throttle_mix() const override;

    MAV_VTOL_STATE get_mav_vtol_state() const override;

protected:

    enum {
        TRANSITION_AIRSPEED_WAIT,
        TRANSITION_TIMER,
        TRANSITION_DONE
    } transition_state;

    // timer start for transition
    uint32_t transition_start_ms;
    uint32_t transition_low_airspeed_ms;

    // last throttle value when active
    float last_throttle;

};

