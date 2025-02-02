#****************************************************************************
#* setup.py for zuspec-arl-eval
#****************************************************************************
import os
import sys
from setuptools import Extension, find_namespace_packages


proj_dir = os.path.dirname(os.path.abspath(__file__))

try:
    sys.path.insert(0, os.path.join(proj_dir, "python/zsp_arl_eval"))
    from __version__ import VERSION
    version=VERSION
except ImportError as e:
    print("Import error: %s" % str(e))
    version="0.0.1"

isSrcBuild = False

try:
    from ivpm.setup import setup
    isSrcBuild = os.path.isdir(os.path.join(proj_dir, "src"))
    print("zuspec-arl-eval: running IVPM SrcBuild")
except ImportError as e:
    from setuptools import setup
    print("zuspec-arl-eval: running non-src build")

# First need to establish where things are
zuspec_arl_eval_dir = proj_dir

ext = Extension("zsp_arl_eval.core",
            sources=[
                os.path.join(zuspec_arl_eval_dir, 'python', "core.pyx"), 
                os.path.join(zuspec_arl_eval_dir, 'python', "EvalBackendClosure.cpp"), 
                os.path.join(zuspec_arl_eval_dir, 'python', "EvalThreadData.cpp"), 
            ],
            language="c++",
            include_dirs=[
                os.path.join(zuspec_arl_eval_dir, 'python'),
                os.path.join(zuspec_arl_eval_dir, 'src', 'include'),
                # os.path.join(packages_dir, 'vsc-dm', 'src', 'include'),
                # os.path.join(packages_dir, 'vsc-dm', 'python'),
                # os.path.join(packages_dir, 'vsc-solvers', 'src', 'include'),
                # os.path.join(packages_dir, 'vsc-solvers', 'python'),
                # os.path.join(packages_dir, 'zuspec-arl-dm', 'src', 'include'),
                # os.path.join(packages_dir, 'zuspec-arl-dm', 'python'),
                # os.path.join(packages_dir, 'pyapi-compat-if', 'src', 'include'),
                # os.path.join(packages_dir, 'pyapi-compat-if', 'build', 'include'),
                # os.path.join(packages_dir, 'pyapi-compat-if', 'python'),
                # os.path.join(packages_dir, 'debug-mgr', 'src', 'include'),
                # os.path.join(packages_dir, 'debug-mgr', 'python'),
            ]
        )
ext.cython_directives={'language_level' : '3'}

setup_args = dict(
  name = "zuspec-arl-eval",
  version=version,
  packages=['zsp_arl_eval'],
  package_dir = {'' : 'python'},
  author = "Matthew Ballance",
  author_email = "matt.ballance@gmail.com",
  description = ("Core ARL data model library"),
  long_description = """
  Provides a library interface for creating and evaluating ARL models at an API level
  """,
  license = "Apache 2.0",
  keywords = ["SystemVerilog", "Verilog", "RTL", "Python"],
  url = "https://github.com/zuspec/zuspec-arl-eval",
  install_requires=[
    'debug-mgr',
    'pyapi-compat-if',
    'vsc-dm',
    'vsc-solvers',
    'zuspec-arl-dm',
  ],
  entry_points={
    'ivpm.pkginfo': [
        "zuspec-arl-eval = zsp_arl_eval.pkginfo:PkgInfo"
    ]
  },
  setup_requires=[
    'setuptools_scm',
    'cython',
  ],
  ext_modules=[ ext ]
)

if isSrcBuild:
    setup_args["ivpm_extdep_pkgs"] = [
        "pyapi-compat-if",
        "vsc-solvers",
        "zuspec-arl-dm"]
    setup_args["ivpm_extra_data"] = {
        "zsp_arl_eval": [
            ("src/include", "share"),
            ("build/{libdir}/{libpref}zsp-arl-eval{dllext}", ""),
        ]
    }

setup(**setup_args)
