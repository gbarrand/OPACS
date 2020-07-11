#if   defined(UNIX)
#include <LocPixU.ic>
#elif defined(VMS)
#include <LocPixV.ic>
#elif defined(NT)
#include <LocPixN.ic>
#else
Define cpp macro UNIX, VMS or NT.
#endif
