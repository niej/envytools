# Copyright © 2020 Rob Clark

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

project(
  'envytools',
  'c',
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>= 0.48',
  default_options : ['c_std=c99', 'warning_level=2'],
)

cc = meson.get_compiler('c')

c_args = [
  '-D_GNU_SOURCE',
]

foreach h : ['xlocale.h', 'linux/futex.h', 'endian.h', 'dlfcn.h', 'execinfo.h', 'sys/shm.h', 'cet.h']
  if cc.check_header(h)
    c_args += '-DHAVE_@0@'.format(h.to_upper().underscorify())
  endif
endforeach

_trial = [
  '-Werror=return-type',
  '-Werror=empty-body',
  '-Werror=incompatible-pointer-types',
  '-Werror=int-conversion',
  '-Wimplicit-fallthrough',
  '-Wno-missing-field-initializers',
  '-Wno-format-truncation',
  '-fno-math-errno',
  '-fno-trapping-math',
  '-Qunused-arguments',
  '-Wno-unused-parameter',
  '-Wno-sign-compare',
  '-Wno-pointer-sign',
  '-Wno-unused-function',
  '-fno-common',
]
foreach a : _trial
  if cc.has_argument(a)
    c_args += a
  endif
endforeach

foreach a : c_args
  add_project_arguments(a, language: 'c')
endforeach

no_override_init_args = []
# TODO once we've sync'd back warnings fixes from mesa, we should switch
# to using a similar set of warning flags

# compat with mesa's meson.build.. all this tree builds is freedreno
# tools:
with_tools = ['freedreno']
inc_include = include_directories('.')
inc_gallium = inc_include
inc_src = include_directories('stub-mesa')
inc_util = inc_src

null_dep = dependency('', required : false)
idep_mesautil = null_dep
idep_nir = null_dep

inc_freedreno = include_directories(['.', './registers', './common'])
inc_freedreno_rnn = include_directories('rnn')

rnn_src_path = meson.source_root() + '/registers'
rnn_install_path = get_option('datadir') + '/freedreno/registers'
rnn_path = rnn_src_path + ':' + get_option('prefix') + '/' + rnn_install_path

dep_lua = dependency('lua53', required: false)
if not dep_lua.found()
  dep_lua = dependency('lua52', required: false)
endif
if not dep_lua.found()
  dep_lua = dependency('lua', required: true)
endif

dep_libarchive = dependency('libarchive', required: true)
dep_libxml2 = dependency('libxml-2.0', required: false)
prog_gzip = find_program('gzip', required: false)

install_fd_decode_tools = true

prog_python = import('python').find_installation('python3')
prog_bison = find_program('bison', required: true)
prog_flex = find_program('flex', required: true)

subdir('stub-mesa/util')

# TODO in mesa, move other dependencies into src/freedreno/meson.build
# and out of individual subdirectories.  For envytools tree, we want
# to make lua/libarchive required, whereas in mesa they are optional.
# The easiest way to accomplish that would be to move them here.

subdir('registers')
subdir('isa')
subdir('ir2')
subdir('ir3')
subdir('rnn')
subdir('decode')
subdir('afuc')

