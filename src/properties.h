/*
 * Author: kayo
 */

#ifndef KAYOVM_PROPERTIES_H
#define KAYOVM_PROPERTIES_H

#include "kayo.h"

/*
 * System properties. The following properties are guaranteed to be defined:
 * java.version         Java version number
 * java.vendor          Java vendor specific string
 * java.vendor.url      Java vendor URL
 * java.home            Java installation directory
 * java.class.version   Java class version number
 * java.class.path      Java classpath
 * os.name              Operating System Name
 * os.arch              Operating System Architecture
 * os.version           Operating System Version
 * file.separator       File separator ("/" on Unix)
 * path.separator       Path separator (":" on Unix)
 * line.separator       Line separator ("\n" on Unix)
 * user.name            User account name
 * user.home            User home directory
 * user.dir             User's current working directory
 */
static const char *properties[][2] = {  // todo
        { "java.version",         "1.0.0" },
        { "java.vendor",          "kayo" },
        { "java.vendor.url",      "doesn't have"},
        { "java.home",            javaHome }, // options.AbsJavaHome // todo
        { "java.class.version",   "52.0"}, // todo
        { "java.class.path",      classpath }, // heap.BootLoader().ClassPath().String() // todo
//            { "java.awt.graphicsenv", "sun.awt.CGraphicsEnvironment"}, // todo
        { "os.name",              "" },   // todo runtime.GOOS
        { "os.arch",              "" }, // todo runtime.GOARCH
        { "os.version",           "" },             // todo
        { "file.separator",       "\\" },            // todo os.PathSeparator
        { "path.separator",       ";" },            // todo os.PathListSeparator
        { "line.separator",       "\r\n" }, // System.out.println最后输出换行符就会用到这个  // todo
        { "user.name",            "" },             // todo
        { "user.home",            "" },             // todo
        { "user.dir",             "." },            // todo
        { "user.country",         "CN" },           // todo
        { "file.encoding",        "UTF-8" },
        { "sun.stdout.encoding",  "UTF-8" },
        { "sun.stderr.encoding",  "UTF-8" },
};

#endif //KAYOVM_PROPERTIES_H
