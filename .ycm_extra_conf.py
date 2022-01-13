import os
import ycm_core

flags = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Wno-long-long',
    '-Wno-variadic-macros',
    '-fexceptions',
    '-ferror-limit=10000',
    '-DNDEBUG',
    '-D_XOPEN_SOURCE_EXTENDED',
    '-std=c99',
    '-xc',
    '-I/usr/local/opt/ncurses/include',
    '-isystem/usr/include/'
    # ,
    # '-internal-isystem/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/local/include',
    # '-internal-externc-isystem/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include'
]

SOURCE_EXTENSIONS = ['.cpp', '.cxx', '.cc', '.c', ]


def FlagsForFile(filename, **kwargs):
    return {
        'flags': flags,
        'do_cache': True
    }
