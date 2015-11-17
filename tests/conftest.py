# -*- coding: utf-8 -*-

import os
import subprocess
import time
from tempfile import NamedTemporaryFile

import pytest


class XvfbProcess(object):

    # Make sure XVFB is running with the same arguments, specifically
    # screen size and color depth.
    def __init__(self, display=None, width=1024, height=800, colors=24):
        if display is None:
            display = os.environ.get('DISPLAY')
        self.display = display
        self.width = width
        self.height = height
        self.colors = colors


class XtermProcessInfo(object):
    def __init__(self, proc, window_id):
        self.proc = proc
        self.window_id = window_id


@pytest.fixture(scope='session')
def xvfb():
    return XvfbProcess()


@pytest.yield_fixture(scope='function')
def xterm_window(xvfb):
    with NamedTemporaryFile() as tf:
        proc = subprocess.Popen(
            ['xterm', '-T', 'My window title',
             '-e', 'echo "$WINDOWID" > "{}"; bash'.format(tf.name)])
        time.sleep(.100)
        with open(tf.name, 'rb') as fp:
            window_id = int(fp.read())
    yield XtermProcessInfo(proc=proc, window_id=window_id)
    proc.terminate()


@pytest.fixture(scope='function')
def xdo(xvfb):
    from xdo import Xdo
    return Xdo(xvfb.display)
