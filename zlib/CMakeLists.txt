
add_library(zlibsrc minizip/iowin32.c minizip/iowin32.h minizip/miniunz.c
        minizip/unzip.c minizip/unzip.h minizip/zip.h minizip/crypt.h minizip/ioapi.h minizip/ioapi.c
        minizip/mztools.h minizip/mztools.c minizip/zip.c
        zlib.h crc32.h crc32.c zutil.h inflate.h inflate.c
        inffast.h inffixed.h inftrees.h adler32.c compress.c deflate.h deflate.c
        gzclose.c gzguts.h gzlib.c gzread.c gzwrite.c infback.c inffast.c
        inftrees.c trees.c trees.h uncompr.c zconf.h zutil.c)