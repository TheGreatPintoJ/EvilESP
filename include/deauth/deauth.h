#ifndef DEAUTH_H
#define DEAUTH_H

#include <Arduino.h>

void start_deauth(int wifi_number, uint16_t reason);
void stop_deauth();

extern int eliminated_stations;
extern int deauth_type;

#endif