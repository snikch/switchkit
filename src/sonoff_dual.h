#ifdef SONOFF_DUAL
#include <SonoffDual.h>
bool sonoffRelayOne = false;
bool sonoffRelayTwo = false;
void setSonoffRelayState()
{
    SonoffDual.setRelays(sonoffRelayOne, sonoffRelayTwo);
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
