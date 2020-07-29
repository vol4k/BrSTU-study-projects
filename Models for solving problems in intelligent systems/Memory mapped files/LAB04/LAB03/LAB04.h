#pragma once

#include "resource.h"


// Tell the compiler to put this initialized variable in its own Shared 
// section so it is shared by all instances of this application.
#pragma data_seg("Shared")
volatile LONG copieCount = 0;
#pragma data_seg()

// Tell the linker to make the Shared section readable, writable, and shared.
#pragma comment(linker, "/Section:Shared,RWS")