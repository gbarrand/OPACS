#if   defined(UNIX)
#include <FStringU.ic>
#elif defined(VMS)
#include <FStringV.ic>
#elif defined(NT)
#include <FStringN.ic>
#else
Define cpp macro UNIX, VMS or NT.
#endif
