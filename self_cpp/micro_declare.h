/// file relative with micro definition 


#pragma once 

#ifdef   _DEBUG
#define  Dump()  (abort())
#else
#define  Dump()  (exit())
#endif


#define LO_BYTE(w)           ((BYTE)(((w)) & 0xff))
#define HI_BYTE(w)           ((BYTE)((((w)) >> 8) & 0xff))
#define MASK(n) (1<<(n))
#define setBit(x, n) ((x) |= MASK(n)) 
#define IsBitSet(x, n) ( ((x) & MASK(n)) != 0 )



#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// call at the begin
// _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

// call at the program exist
//_CrtDumpMemoryLeaks();

// locate memory leak place 
//_CrtSetBreakAlloc(18);