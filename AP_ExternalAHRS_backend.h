
/*
  parent class for ExternalAHRS backends
 */

#pragma once

#include "AP_ExternalAHRS.h" 

#if HAL_EXTERNAL_AHRS_ENABLED

class AP_ExternalAHRS_backend {
public:
    AP_ExternalAHRS_backend(AP_ExternalAHRS *frontend, AP_ExternalAHRS::state_t &state);

    // get serial port number, -1 for not enabled
    virtual int8_t get_port(void) const { return -1; }

    // Get model/type name
    virtual const char* get_name() const = 0;

    // Accessors for AP_AHRS

    // If not healthy, none of the other API's can be trusted.
    // Example: Serial cable is severed.
    virtual bool healthy(void) const = 0;
    // The communication interface is up and the device has sent valid data.
    virtual bool initialised(void) const = 0;
    virtual bool pre_arm_check(char *failure_msg, uint8_t failure_msg_len) const = 0;
    virtual void get_filter_status(nav_filter_status &status) const {}
    virtual void send_status_report(class GCS_MAVLINK &link) const {}

    // Check for new data.
    // This is used when there's not a separate thread for EAHRS.
    // This can also copy interim state protected by locking.
    virtual void update() = 0;

    // Return the number of GPS sensors sharing data to AP_GPS.
    virtual uint8_t num_gps_sensors(void) const = 0;

protected:
    AP_ExternalAHRS::state_t &state;
    uint16_t get_rate(void) const;
    bool option_is_set(AP_ExternalAHRS::OPTIONS option) const;

    // set default of EAHRS_SENSORS
    void set_default_sensors(uint16_t sensors) {
        frontend.set_default_sensors(sensors);
    }

    /*
      return true if the GNSS is disabled
     */
    bool gnss_is_disabled(void) const {
        return frontend.gnss_is_disabled;
    }

    /*
      return true when we are in fixed wing flight
     */
    bool in_fly_forward(void) const;

private:
    AP_ExternalAHRS &frontend;
};

#endif  // HAL_EXTERNAL_AHRS_ENABLED

