# Copyright © 2017 Intel Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

inc_util = include_directories('.')

files_mesa_util = files(
  'bitscan.h',
  'bitset.h',
  'compiler.h',
  'half_float.c',
  'half_float.h',
  'hash_table.c',
  'hash_table.h',
  'list.h',
  'log.c',
  'log.h',
  'macros.h',
  'os_file.h',
  'ralloc.c',
  'ralloc.h',
  'set.c',
  'set.h',
  'softfloat.c',
  'softfloat.h',
  'u_debug.h',
  'u_endian.h',
  'u_math.c',
  'u_math.h',
)


deps_for_libmesa_util = [
]

_libmesa_util = static_library(
  'mesa_util',
  [files_mesa_util],
  include_directories : [inc_include, inc_src, inc_gallium],
  dependencies : deps_for_libmesa_util,
  gnu_symbol_visibility : 'hidden',
  build_by_default : false
)

idep_mesautil = declare_dependency(
  link_with : _libmesa_util,
  include_directories : inc_util,
#  dependencies : [dep_zlib, dep_clock, dep_thread, dep_atomic, dep_m, dep_valgrind],
)

