#define BUILD_LIB
#include "S5dll_api.h"

// Redefined by s5_inc.h
//**********************
#ifdef SERIAL_NO_HANDSHAKE
#undef SERIAL_NO_HANDSHAKE
#endif

#ifdef SERIAL_RTS_HANDSHAKE
#undef SERIAL_RTS_HANDSHAKE
#endif

#ifdef CLR_RX_BUFFER
#undef	CLR_RX_BUFFER
#endif

#ifdef CLR_TX_BUFFER
#undef	CLR_TX_BUFFER
#endif
//**********************
