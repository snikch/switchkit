#ifdef SONOFF_DUAL
//#include <SonoffDual.h>
bool sonoffRelayOne = false;
bool sonoffRelayTwo = false;

void setRelays() {
  byte b = 0;
  if (sonoffRelayTwo) b++;
  if (sonoffRelayOne) b += 2;

  Serial.write((byte)0xA0);
  Serial.write((byte)0x04);
  Serial.write((byte)b);
  Serial.write((byte)0xA1);;

  Serial.flush();
}
void setSonoffRelayState()
{
  setRelays();
    //SonoffDual.setRelays(sonoffRelayOne, sonoffRelayTwo);
}
void dualDidChangeStateOne(bool state)
{
    sonoffRelayOne = state;
    setSonoffRelayState();
}
void dualDidChangeStateTwo(bool state)
{
    sonoffRelayTwo = state;
    setSonoffRelayState();
}
#endif
