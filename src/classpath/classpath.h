#ifndef CABIN_CLASSPATH_H
#define CABIN_CLASSPATH_H

#include <optional>
#include "../cabin.h"

void setBootstrapClasspath(const char *bcp);

void setMainClasspath(const char *cp);
const char *getMainClasspath();

void initClasspath();

/*
 * Read JDK 类库中的类，不包括Array Class.
 * xxx/xxx/xxx
 */
std::optional<std::pair<u1 *, size_t>> readBootClass(const utf8_t *class_name);

#endif //CABIN_CLASSPATH_H
