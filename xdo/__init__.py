import os
import ctypes
from collections import namedtuple

from .xdo import libxdo as _libxdo


mouse_location = namedtuple('mouse_location', 'x,y,screen_num')
mouse_location2 = namedtuple('mouse_location2', 'x,y,screen_num,window')


class Xdo(object):
    def __init__(self, display=None):
        if display is None:
            display = os.environ.get('DISPLAY', '')
        self._xdo = _libxdo.xdo_new(display)

    def version(self):
        return _libxdo.xdo_version()

    def move_mouse(self, x, y, screen=0):
        """
        Move the mouse to a specific location.

        :param x: the target X coordinate on the screen in pixels.
        :param y: the target Y coordinate on the screen in pixels.
        :param screen: the screen (number) you want to move on.
        """
        # todo: apparently the "screen" argument is not behaving properly
        #       and sometimes even making the interpreter crash..
        #       Figure out why (changed API / using wrong header?)

        # >>> xdo.move_mouse(3000,200,1)

        # X Error of failed request:  BadWindow (invalid Window parameter)
        #   Major opcode of failed request:  41 (X_WarpPointer)
        #   Resource id in failed request:  0x2a4fca0
        #   Serial number of failed request:  25
        #   Current serial number in output stream:  26

        # Just to be safe..
        screen = 0

        _libxdo.xdo_move_mouse(self._xdo, x, y, screen)

    def move_mouse_relative_to_window(self, window, x, y):
        """
        Move the mouse to a specific location relative to the top-left corner
        of a window.

        :param x: the target X coordinate on the screen in pixels.
        :param y: the target Y coordinate on the screen in pixels.
        """
        _libxdo.xdo_move_mouse_relative_to_window(self._xdo, window, x, y)

    def move_mouse_relative(self, x, y):
        """
        Move the mouse relative to it's current position.

        :param x: the distance in pixels to move on the X axis.
        :param y: the distance in pixels to move on the Y axis.
        """
        _libxdo.xdo_move_mouse_relative(self._xdo, x, y)

    def mouse_down(self, window, button):
        """
        Send a mouse press (aka mouse down) for a given button at
        the current mouse location.

        :param window:
            The window you want to send the event to or CURRENTWINDOW
        :param button:
            The mouse button. Generally, 1 is left, 2 is middle, 3 is
            right, 4 is wheel up, 5 is wheel down.
        """
        _libxdo.xdo_mouse_down(self._xdo, window, button)

    def mouse_up(self, window, button):
        """
        Send a mouse release (aka mouse up) for a given button at
        the current mouse location.

        :param window:
            The window you want to send the event to or CURRENTWINDOW
        :param button:
            The mouse button. Generally, 1 is left, 2 is middle, 3 is
            right, 4 is wheel up, 5 is wheel down.
        """
        _libxdo.xdo_mouse_up(self._xdo, window, button)

    def get_mouse_location(self):
        """
        Get the current mouse location (coordinates and screen number).

        :return: a namedtuple with ``x``, ``y`` and ``screen_num`` fields
        """
        x = ctypes.c_int(0)
        y = ctypes.c_int(0)
        screen_num = ctypes.c_int(0)
        _libxdo.xdo_get_mouse_location(
            self._xdo, ctypes.byref(x), ctypes.byref(y),
            ctypes.byref(screen_num))
        return mouse_location(x.value, y.value, screen_num.value)

    def get_window_at_mouse(self):
        """
        Get the window the mouse is currently over
        """
        window_ret = ctypes.c_ulong(0)
        _libxdo.xdo_get_window_at_mouse(self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def get_mouse_location2(self):
        """
        Get all mouse location-related data.

        :return: a namedtuple with ``x``, ``y``, ``screen_num``
            and ``window`` fields
        """
        x = ctypes.c_int(0)
        y = ctypes.c_int(0)
        screen_num_ret = ctypes.c_ulong(0)
        window_ret = ctypes.c_ulong(0)
        _libxdo.xdo_get_mouse_location2(
            self._xdo, ctypes.byref(x), ctypes.byref(y),
            ctypes.byref(screen_num_ret), ctypes.byref(window_ret))
        return mouse_location2(x.value, y.value, screen_num_ret.value,
                               window_ret.value)

    def wait_for_mouse_move_from(self, origin_x, origin_y):
        """
        Wait for the mouse to move from a location. This function will block
        until the condition has been satisified.

        :param origin_x: the X position you expect the mouse to move from
        :param origin_y: the Y position you expect the mouse to move from
        """
        _libxdo.xdo_wait_for_mouse_move_from(self._xdo, origin_x, origin_y)

    def wait_for_mouse_move_to(self, dest_x, dest_y):
        """
        Wait for the mouse to move to a location. This function will block
        until the condition has been satisified.

        :param dest_x: the X position you expect the mouse to move to
        :param dest_y: the Y position you expect the mouse to move to
        """
        _libxdo.xdo_wait_for_mouse_move_from(self._xdo, dest_x, dest_y)

    def click_window(self, window, button):
        """
        Send a click for a specific mouse button at the current mouse location.

        :param window:
            The window you want to send the event to or CURRENTWINDOW
        :param button:
            The mouse button. Generally, 1 is left, 2 is middle, 3 is
            right, 4 is wheel up, 5 is wheel down.
        """
        _libxdo.xdo_click_window(self._xdo, window, button)

    def click_window_multiple(self, window, button, repeat=2, delay=100000):
        """
        Send a one or more clicks for a specific mouse button at the
        current mouse location.

        :param window:
            The window you want to send the event to or CURRENTWINDOW
        :param button:
            The mouse button. Generally, 1 is left, 2 is middle, 3 is
            right, 4 is wheel up, 5 is wheel down.
        :param repeat: number of repetitions (default: 2)
        :param delay: delay between clicks, in microseconds (default: 100k)
        """
        _libxdo.xdo_click_window_multiple(
            self._xdo, window, button, repeat, delay)

    def enter_text_window(self, window, string, delay=12000):
        """
        Type a string to the specified window.

        If you want to send a specific key or key sequence, such as
        "alt+l", you want instead ``send_keysequence_window(...)``.

        :param window:
            The window you want to send keystrokes to or CURRENTWINDOW
        :param string:
            The string to type, like "Hello world!"
        :param delay:
            The delay between keystrokes in microseconds.
            12000 is a decent choice if you don't have other plans.
        """
        return _libxdo.xdo_enter_text_window(self._xdo, window, string, delay)

    def send_keysequence_window(self, window, keysequence, delay=12000):
        """
        Send a keysequence to the specified window.

        This allows you to send keysequences by symbol name. Any combination
        of X11 KeySym names separated by '+' are valid. Single KeySym names
        are valid, too.

        Examples:
          "l"
          "semicolon"
          "alt+Return"
          "Alt_L+Tab"

        If you want to type a string, such as "Hello world." you want to
        instead use xdo_enter_text_window.

        :param window: The window you want to send the keysequence to or
          CURRENTWINDOW
        :param keysequence: The string keysequence to send.
        :param delay: The delay between keystrokes in microseconds.
        """
        _libxdo.xdo_send_keysequence_window(
            self._xdo, window, keysequence, delay)

    def send_keysequence_window_up(self, window, keysequence, delay=12000):
        """Send key release (up) events for the given key sequence"""
        _libxdo.xdo_send_keysequence_window_up(
            self._xdo, window, keysequence, delay=12000)

    def send_keysequence_window_down(self, window, keysequence, delay=12000):
        """Send key press (down) events for the given key sequence"""
        _libxdo.xdo_send_keysequence_window_down(
            self._xdo, window, keysequence, delay=12000)

    def send_keysequence_window_list_do(
            self, window, keys, pressed=1, modifier=None, delay=120000):
        """
        Send a series of keystrokes.

        :param window: The window to send events to or CURRENTWINDOW
        :param keys: The array of charcodemap_t entities to send.
        :param pressed: 1 for key press, 0 for key release.
        :param modifier:
            Pointer to integer to record the modifiers
            activated by the keys being pressed. If NULL, we don't save
            the modifiers.
        :param delay:
            The delay between keystrokes in microseconds.
        """
        # todo: how to properly use charcodes_t in a nice way?
        _libxdo.xdo_send_keysequence_window_list_do(
            self._xdo, window, keys, len(keys), pressed, modifier, delay)

    def select_window_with_click(self):
        window_ret = ctypes.c_ulong(0)
        _libxdo.xdo_select_window_with_click(
            self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def __del__(self):
        _libxdo.xdo_free(self._xdo)
