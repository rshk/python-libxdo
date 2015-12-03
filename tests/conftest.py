# -*- coding: utf-8 -*-
import os
import subprocess
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
def xterm_window(xvfb, tmpdir):
    """
    Create an xterm window test fixture.  This fixture opens a new xterm
    window and yields its the X window id.  Upon test completion the xterm
    process is cleaned up.
    :param xvfb:
    :return:
    """
    xterm_pipe_path = tmpdir.join('xterm_pipe').strpath
    xterm_proc = None

    try:
        os.mkfifo(xterm_pipe_path)
        xterm_proc = subprocess.Popen([
            'xterm', '-T', 'My window title', '-e',
            'echo "$WINDOWID" > "{}"; bash'.format(xterm_pipe_path)
        ])
        with open(xterm_pipe_path, 'r') as pipe:
            window_id = int(pipe.read())
        yield XtermProcessInfo(proc=xterm_proc, window_id=window_id)
    finally:
        if xterm_proc:
            xterm_proc.terminate()


@pytest.fixture(scope='function')
def xdo(xvfb):
    from xdo import Xdo
    return Xdo(xvfb.display)
