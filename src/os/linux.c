#ifdef __linux__
#include <unistd.h>

int processor_number()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

int page_size()
{
    JVM_PANIC("pageSize");
}

const char *os_name()
{
    struct utsname x;
    uname(&x);
    return x.sysname;
}

const char *os_arch()
{
    JVM_PANIC("osArch");
}

const char *get_file_separator()
{
    return "/";
}

const char *get_path_separator()
{
    return ":";
}

const char *get_line_separator()
{
    return "\n";
}

// string getTimeZone()
// {
//     JVM_PANIC("getTimeZone");
// }

#endif
