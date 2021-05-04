# # -*- coding: utf-8 -*-

from distutils.core import setup

setup(
    name='rtpg',
    version='',
    package_dir={'': 'python'},
    packages=['rtpg'],
    # py_modules=[''],
    package_data={'rtpg': ['_rtpg_py.so', 'librtpg.so']}
)
