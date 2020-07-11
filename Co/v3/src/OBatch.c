#if   defined(UNIX)
#include <OBatchU.ic>
#elif defined(VMS)
#include <OBatchV.ic>
#elif defined(NT)
#include <OBatchN.ic>
#else
Define cpp macro UNIX, VMS or NT.
#endif
