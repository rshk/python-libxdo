##########################
Python bindings for libxdo
##########################


libxdo is the library behind
`xdotool <https://github.com/jordansissel/xdotool>`__, a tool to "fake
keyboard/mouse input, window management, and more".

This package aims to provide ctypes-based Python bindings (and a more
pythonic way to use the C library, of course..)

**WARNING** The project is still work in progress! Only a small part of
the library is supported, and tests / documentation are missing. Use
with care (and make sure to report any issue you should find!)


Documentation
=============

Documentation is hosted on Read The Docs:

http://python-libxdo.readthedocs.org/

**Note:** there currently is a problem building the documentation on RTD
due to missing libxdo library (which makes importing ``xdo.xdo`` fail).
For this reason, a (locally-generated) copy of the documentation is
hosted on github pages:

https://rshk.github.io/python-libxdo


Example usage
=============

Ask the user to click a window with the mouse, then write "Python
rocks!" to that window.

.. code:: python

    from xdo import Xdo
    xdo = Xdo()
    win_id = xdo.select_window_with_click()
    xdo.enter_text_window(win_id, 'Python rocks!')


Compatibility
=============

The library is currently tested on:

+---------------------------------------+----------------+--------------------+--------------+---------------+
| OS                                    | Architecture   | libxdo version     | Date         | Test result   |
+=======================================+================+====================+==============+===============+
| Debian GNU/Linux "jessie" (testing)   | amd64          | 3.20130111.1-3.1   |              | ✓ success     |
+---------------------------------------+----------------+--------------------+--------------+---------------+
| Archlinux                             | amd64          | 3.20150503.1       | 2015-11-16   | ✓ success     |
+---------------------------------------+----------------+--------------------+--------------+---------------+


Installation
============

The package is available on pypi
https://pypi.python.org/pypi/python-libxdo/

Install via pip:

::

    pip install python-libxdo


Install the tarball directly from github:

::

    pip install https://github.com/rshk/python-libxdo/tarball/master


Install in development mode from git:

::

    pip install -e 'git+https://github.com/rshk/python-libxdo.git#egg=python-libxdo'
