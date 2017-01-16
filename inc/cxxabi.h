#ifndef INC_CXXABI_H_
#define INC_CXXABI_H_


extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);
extern "C" void __cxa_finalize(void *f);


#endif /* INC_CXXABI_H_ */
