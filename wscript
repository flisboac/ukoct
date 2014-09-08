#!/usr/bin/env python

import waflib

APPNAME = 'ukoct'
VERSION = '0.1.0'
MAJOR = 0

top = '.'
out = 'build'
src = 'src'
inc = 'include'
ext = 'ext'
ext_include = ext + '/include'
spec = 'spec'
test = 'test'

LIBNAME = APPNAME + str(MAJOR)
SPECNAME = APPNAME + '_spec'

BUILDS = {
	'default': 'full-debug',
	'all': {
		'all': {
			'load': 'tool',
			#'load': ['tool1', 'tool2', {'input': 'tool1', 'mandatory': True}],
			'load': 'compiler_cxx waf_unit_test',
			'find_program': [
				# e.g. {'filename': 'diff', 'mandatory': True},
			],
			'check': [
				{
					'compiler': 'cxx',
					'lib': 'OpenCL',
					'mandatory': True,
					'uselib_store': 'OPENCL',
				},
			],
			'includes': [inc, src, ext_include],
			'cxxflags CXX_NAME == "gcc"': ['--std=c++11'],
		},

		#LIBNAME: {
		#	'features': 'cxx cxxshlib', # e.g. 'cxx cxxstlib',
		#	'source': [
        #		# e.g. src + '/cnf.cpp',
		#		src + '/ukoct.cpp',
        #    ],
		#},

		APPNAME: {
			'features': 'cxx cxxprogram', # e.g. 'cxx cxxprogram',
			'use': ['OPENCL'],
			'source': [src + '/main.cpp'],
		},

		#SPECNAME: {
		#	'features': 'cxxprogram',
		#	'use': APPNAME,
		#	'source': [spec + '/uksat_spec.cpp'],
		#}
		
		# --- UNIT TESTS ---
		
		'@' + test + '/plas': {
			'features': 'cxx cxxprogram test',
			'source': [test + '/plas/*.cpp']
		},
		
		'@' + test + '/cpu': {
			'features': 'cxx cxxprogram test',
			'source': [test + '/cpu/*.cpp']
		},
		
		'@' + test + '/opencl-common': {
			'features': 'cxx cxxprogram test',
			'source': [test + '/opencl-common/*.cpp']
		},
	},

	'debug': {
		'all': {
			'+cxxflags  CXX_NAME == "gcc"  ': ['-g', '-O1'],
			'+linkflags CXX_NAME == "gcc"  ': ['-g'],

			'+cxxflags  CXX_NAME == "msvc" ': ['/Zi'],
			'+linkflags CXX_NAME == "msvc" ': ['/DEBUG'],
		},
	},

	'full-debug': {
		'all': {
			'+cxxflags  CXX_NAME == "gcc"  ': ['-g3', '-fno-inline', '-O0'],
			'+linkflags CXX_NAME == "gcc"  ': ['-g3'],

			'+cxxflags  CXX_NAME == "msvc" ': ['/Zi'],
			'+linkflags CXX_NAME == "msvc" ': ['/DEBUG'],
		},
	},

	'release': {
		'all': {
			'+cxxflags  CXX_NAME == "gcc"  ': ['-O2', '-s'],

			'+cxxflags  CXX_NAME == "msvc" ': ['/O2'],
		}
	},

	'profile': {
		'all': {
			'+cxxflags  CXX_NAME == "gcc"  ': ['-g', '-pg'],
			'+linkflags CXX_NAME == "gcc"  ': ['-g', '-pg'],

			'+linkflags CXX_NAME == "msvc" ': ['/PROFILE'],  # ... only available only in Enterprise (team development) versions. (sic)
		},
	},

	'profile-valgrind': {
		'all': {
			'+cxxflags  CXX_NAME == "gcc"  ': ['-O2', '-g'],
			'+linkflags CXX_NAME == "gcc"  ': ['-g'],
		},
		'@echo': {
			'target': None,
		}
	},
}


def options(ctx):
	ctx.load('wafbuild')


def configure(ctx):
    ctx.load('wafbuild')


def build(ctx):
	ctx.load('wafbuild')
	from waflib.Tools import waf_unit_test
	ctx.add_post_fun(waf_unit_test.summary)
	#ctx.add_post_fun(lambda ctx: print(ctx.utest_results))
	ctx.add_post_fun(waf_unit_test.set_exit_code)


def clsources(ctx):
	"""Generates OpenCL source code from the includes."""
	# TODO Use regex instead
	# this is a poor-man's hack, it relies on the assumption that files
	# start and end with parenthesis. Put a comment before, and you generate
	# wrong code. But the source is under my control anyways.
	float_srcfolder = ctx.path.find_node(inc + "/ukoct/opencl/float/sources")
	common_clsources = ctx.path.ant_glob(inc + "/ukoct/opencl/common/**/*.cl")
	float_clsources = [
		float_srcfolder.find_node("defs.inc.cl"),
		float_srcfolder.find_node("coherent.inc.cl"),
		float_srcfolder.find_node("consistent.inc.cl"),
		float_srcfolder.find_node("intConsistent.inc.cl"),
		float_srcfolder.find_node("closed.inc.cl"),
		float_srcfolder.find_node("stronglyClosed.inc.cl"),
	]
	float_clsource = []
	for node in common_clsources:
		source = node.read()
		float_clsource.append(source[source.find("(") + 1 : source.rfind(")")])
	for node in float_clsources:
		source = node.read()
		float_clsource.append(source[source.find("(") + 1 : source.rfind(")")])
	ctx.path.make_node("float.cl").write(''.join(float_clsource))


def generate_operator(ctx):
	pass

#def distclean(ctx):
#	import shutil
#	pycache_node = ctx.path.find_node('__pycache__')
#	shutil.rmtree(pycache_node.abspath())
#	for node in ctx.path.ant_glob('*.log'):
#		node.delete()
#	ctx.execute()
