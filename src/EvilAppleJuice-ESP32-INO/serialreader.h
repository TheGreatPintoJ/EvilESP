#ifndef SERIALREADER_H
#define SERIALREADER_H

extern String serialInputString;
extern bool serialStringComplete;

void serialCommandRead();
void serialHandleCommand(String inputCMD);

#endif