#include <inc/cxxabi.h>
#include <inc/console.h>

//void *__dso_handle;

extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso)
{
	kprintf("cxa_atexit\n");
	return 0;
}

extern "C" void __cxa_finalize(void *f)
{
	kprintf("cxa_finalize\n");
}

void operator delete(void * p)
{
	kprintf("operator delete\n");
}

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
	kprintf("Pure virtual call\n");
}
