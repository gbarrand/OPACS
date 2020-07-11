#if   defined(UNIX)
#include <hgU.ic>
#elif defined(VMS)
#include <hgV.ic>
#elif defined(NT)
#include <hgN.ic>
#else
Define cpp macro UNIX, VMS or NT.
#endif
