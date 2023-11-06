extern "C" {
   #include "cfe.h"
}


extern "C" int32_t CppBase_Init(void);
int32_t CppBase_Init(void) {

   OS_printf("Cromulence Base C++ Library Initialized.\n");
   return OS_SUCCESS;

}
