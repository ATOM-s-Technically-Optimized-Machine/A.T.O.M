#ifndef ATOM_PLATFORM_H_
    #define ATOM_PLATFORM_H_

#ifdef _WIN32
    #define lib_open(path)       LoadLibraryA(path)
    #define lib_sym(handle, name) GetProcAddress(handle, name)
    #define lib_close(handle)    FreeLibrary(handle)
    #define LIB_EXT ".dll"
    #define ATOM_EXPORT __declspec(dllexport)
#else
    #define lib_open(path)       dlopen(path, RTLD_NOW)
    #define lib_sym(handle, name) dlsym(handle, name)
    #define lib_close(handle)    dlclose(handle)
    #define LIB_EXT ".so"
    #define ATOM_EXPORT __attribute__((visibility("default")))
#endif

#endif /* !ATOM_PLATFORM_H_ */
