import os

from setuptools import find_packages, setup

version = '0.1.2a1'
install_requires = ['six']

here = os.path.dirname(__file__)

with open(os.path.join(here, 'README.rst')) as fp:
    longdesc = fp.read()

with open(os.path.join(here, 'CHANGELOG.rst')) as fp:
    longdesc += "\n\n" + fp.read()

setup(
    name='python-libxdo',
    version=version,
    packages=find_packages(),
    url='https://github.com/rshk/python-libxdo',

    # 3-clause BSD, same as xdotool
    license='BSD License',

    author='Samuele Santi',
    author_email='samuele@samuelesanti.com',
    description='Python bindings for libxdo',
    long_description='',
    install_requires=install_requires,
    # test_suite='tests',
    classifiers=[
        "License :: OSI Approved :: BSD License",  # 3-clause

        # "Development Status :: 1 - Planning",
        # "Development Status :: 2 - Pre-Alpha",
        "Development Status :: 3 - Alpha",
        # "Development Status :: 4 - Beta",
        # "Development Status :: 5 - Production/Stable",
        # "Development Status :: 6 - Mature",
        # "Development Status :: 7 - Inactive",

        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",

        # Only CPython is supported at the moment
        "Programming Language :: Python :: Implementation :: CPython",
        # "Programming Language :: Python :: Implementation :: PyPy",
    ],
    package_data={'': ['README.rst', 'LICENSE', 'CHANGELOG.rst']})
