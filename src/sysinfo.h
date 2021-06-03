#ifndef CABIN_SYSINFO_H
#define CABIN_SYSINFO_H

int processor_number();
int page_size();

// 返回操作系统的名称。e.g. window 10
const char *os_name();

// 返回操作系统的架构。e.g. amd64
const char *os_arch();

const char *get_file_separator();
const char *get_path_separator();
const char *get_line_separator();

/*
 * 大端(big endian):低地址存放高字节
 * 小端(little endian):低字节存放低字节
 */
static inline bool is_big_endian()
{
    static const union {
        char c[4]; 
        unsigned long l;
    } endian_test = { { 'l', '?', '?', 'b' } };

    return ((char) endian_test.l) == 'b';
}

#endif // CABIN_SYSINFO_H