phome#pragma once

#ifdef USE_ESP32

#include "esphome/core/component.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/core/automation.h"

#include "esphome/core/version.h"
#if __has_include("esphome/core/macros.h")
#include "esphome/core/macros.h" // VERSION_CODE
#else
#define VERSION_CODE(major, minor, patch) ((major) << 16 | (minor) << 8 | (patch))
#endif

namespace esphome {
namespace ble_gateway {

class BLEGateway : public Component, public esp32_ble_tracker::ESPBTDeviceListener {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void add_callback(std::function<void(const esp32_ble_tracker::ESPBTDevice &, esphome::string)> &&callback) { this->callback_.add(std::move(callback)); }

#if ESPHOME_VERSION_CODE >= VERSION_CODE(2025, 6, 0)
  static esphome::string scan_result_to_hci_packet_hex(const esp32_ble::BLEScanResult &scan_result);
#else
  static esphome::string scan_result_to_hci_packet_hex(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &scan_result);
#endif
  void dump_config() override;
  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;
  bool get_discovery() const { return this->discovery_; }
  void set_discovery(bool discovery) { this->discovery_ = discovery; }
  void add_device(uint64_t device);
  void set_devices(esphome::string devices);
 protected:
  bool discovery_{false};
  std::vector<uint64_t> devices_{};
  CallbackManager<void(const esp32_ble_tracker::ESPBTDevice &, esphome::string)> callback_{};
};

class BLEGatewayBLEAdvertiseTrigger : public Trigger<const esp32_ble_tracker::ESPBTDevice &, esphome::string> {
 public:
  explicit BLEGatewayBLEAdvertiseTrigger(BLEGateway *parent) {
   parent->add_callback([this](const esp32_ble_tracker::ESPBTDevice &value, esphome::string packet) {
    this->trigger(value, packet); });
  }
};

}  // namespace ble_gateway
}  // namespace esphome

#endif
