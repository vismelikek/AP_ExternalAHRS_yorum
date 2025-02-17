
/*
  support for serial connected AHRS systems
 */

#pragma once

#include "AP_ExternalAHRS_config.h"

#if AP_EXTERNAL_AHRS_VECTORNAV_ENABLED

#include "AP_ExternalAHRS_backend.h"

class AP_ExternalAHRS_VectorNav : public AP_ExternalAHRS_backend {

public:
    AP_ExternalAHRS_VectorNav(AP_ExternalAHRS *frontend, AP_ExternalAHRS::state_t &state);

    // get serial port number, -1 for not enabled
    int8_t get_port(void) const override;

    // accessors for AP_AHRS
    bool healthy(void) const override;
    bool initialised(void) const override;
    bool pre_arm_check(char *failure_msg, uint8_t failure_msg_len) const override;
    void get_filter_status(nav_filter_status &status) const override;
    void send_status_report(class GCS_MAVLINK &link) const override;

    // check for new data
    void update() override {
        check_uart();
    }

    // Get model/type name
    const char* get_name() const override;

protected:

    uint8_t num_gps_sensors(void) const override {
        return 1;
    }
private:
    AP_HAL::UARTDriver *uart;
    int8_t port_num;
    bool setup_complete;
    uint32_t baudrate;

    void update_thread();
    bool check_uart();

    void process_ins_packet1(const uint8_t *b);
    void process_ins_packet2(const uint8_t *b);
    void process_ahrs_packet(const uint8_t *b);
    void wait_register_responce(const uint8_t register_num);

    uint8_t *pktbuf;
    uint16_t pktoffset;
    uint16_t bufsize;

    struct VN_INS_packet1 *last_ins_pkt1;
    struct VN_INS_packet2 *last_ins_pkt2;

    uint32_t last_pkt1_ms;
    uint32_t last_pkt2_ms;

    enum class TYPE {
        VN_INS,  // Full INS mode, requiring GNSS. Used by VN-2X0 and VN-3X0
        VN_AHRS,  // IMU-only mode, used by VN-1X0
    } type;

    bool has_dual_gnss = false;

    char model_name[25];

    // NMEA parsing for setup
    bool decode(char c);
    bool decode_latest_term();
    struct NMEA_parser {
        char term[25];            // buffer for the current term within the current sentence
        uint8_t term_offset;      // offset within the _term buffer where the next character should be placed
        uint8_t term_number;      // term index within the current sentence
        uint8_t checksum;         // checksum accumulator
        bool term_is_checksum;    // current term is the checksum
        bool sentence_valid;      // is current sentence valid so far
        bool sentence_done;       // true if this sentence has already been decoded
        uint8_t register_number;  // VectorNAV register number were reading
    } nmea;

};

#endif  // AP_EXTERNAL_AHRS_VECTORNAV_ENABLED
