# -*- coding: utf-8 -*-

import pytest

from xdo import (
    MOUSE_LEFT, MOUSE_MIDDLE, MOUSE_RIGHT, MOUSE_WHEEL_DOWN, MOUSE_WHEEL_UP,
    Xdo)

BROKEN_TEST = pytest.mark.xfail(reason='Something is broken', run=False)

XDO_VERSION = Xdo.version_info()


class TestBase(object):

    def test_move_mouse(self, xdo):
        xdo.move_mouse(200, 100)
        assert xdo.get_mouse_location() == (200, 100, 0)

    def test_move_mouse_left_boundary(self, xdo):
        xdo.move_mouse(-10, 100)
        assert xdo.get_mouse_location() == (0, 100, 0)

    def test_move_mouse_top_boundary(self, xdo):
        xdo.move_mouse(100, -10)
        assert xdo.get_mouse_location() == (100, 0, 0)

    def test_move_mouse_right_boundary(self, xdo):
        xdo.move_mouse(9999, 100)
        assert xdo.get_mouse_location() == (1023, 100, 0)

    def test_move_mouse_bottom_boundary(self, xdo):
        xdo.move_mouse(100, 9999)
        assert xdo.get_mouse_location() == (100, 799, 0)

    def test_move_mouse_other_screen(self, xdo):
        xdo.move_mouse(200, 100, 1)
        assert xdo.get_mouse_location() == (200, 100, 1)

    @BROKEN_TEST
    def test_move_mouse_relative_to_window(self, xdo, xterm_window):
        xdo.move_window(xterm_window.window_id, 20, 10)
        xdo.move_mouse_relative_to_window(xterm_window.window_id, 25, 15)

        assert xdo.get_mouse_location() == (20 + 25, 10 + 15, 0)

    def test_move_mouse_relative(self, xdo):
        xdo.move_mouse(20, 10)
        assert xdo.get_mouse_location() == (20, 10, 0)

        xdo.move_mouse_relative(40, 30)
        assert xdo.get_mouse_location() == (20 + 40, 10 + 30, 0)

        xdo.move_mouse_relative(-10, -5)
        assert xdo.get_mouse_location() == (20 + 40 - 10, 10 + 30 - 5, 0)

    def test_mouse_down(self, xdo, xterm_window):
        # todo: how to test the effects?
        xdo.mouse_down(xterm_window.window_id, MOUSE_LEFT)
        xdo.mouse_down(xterm_window.window_id, MOUSE_MIDDLE)
        xdo.mouse_down(xterm_window.window_id, MOUSE_RIGHT)
        xdo.mouse_down(xterm_window.window_id, MOUSE_WHEEL_UP)
        xdo.mouse_down(xterm_window.window_id, MOUSE_WHEEL_DOWN)

    def test_mouse_up(self, xdo, xterm_window):
        # todo: how to test the effects?
        xdo.mouse_up(xterm_window.window_id, MOUSE_LEFT)
        xdo.mouse_up(xterm_window.window_id, MOUSE_MIDDLE)
        xdo.mouse_up(xterm_window.window_id, MOUSE_RIGHT)
        xdo.mouse_up(xterm_window.window_id, MOUSE_WHEEL_UP)
        xdo.mouse_up(xterm_window.window_id, MOUSE_WHEEL_DOWN)

    def test_get_mouse_location(self, xdo):
        xdo.move_mouse(200, 100, 0)

        location = xdo.get_mouse_location()

        assert location == (200, 100, 0)
        assert location.x == 200
        assert location.y == 100
        assert location.screen_num == 0
