# -*- coding: utf-8 -*-

import ctypes
import os
from collections import namedtuple
from ctypes import POINTER

from six.moves import range

from .xdo import libX11 as _libX11
from .xdo import libxdo as _libxdo
from .xdo import (  # noqa
    CURRENTWINDOW, SEARCH_CLASS, SEARCH_CLASSNAME, SEARCH_DESKTOP, SEARCH_NAME,
    SEARCH_ONLYVISIBLE, SEARCH_PID, SEARCH_SCREEN, SEARCH_TITLE, Atom, Screen,
    XdoException, XErrorHandler, charcodemap_t, window_t, xdo_search_t)


mouse_location = namedtuple('mouse_location', 'x,y,screen_num')
mouse_location2 = namedtuple('mouse_location2', 'x,y,screen_num,window')
window_location = namedtuple('window_location', 'x,y,screen')
window_size = namedtuple('window_size', 'width,height')
input_mask = namedtuple('input_mask', 'shift,lock,control,mod1,mod2,mod3,mod4,mod5')  # noqa


# Mouse button constants
MOUSE_LEFT = 1
MOUSE_MIDDLE = 2
MOUSE_RIGHT = 3
MOUSE_WHEEL_UP = 4
MOUSE_WHEEL_DOWN = 5

# Keyboard modifiers
MOD_Shift = 1 << 0
MOD_Lock = 1 << 1
MOD_Control = 1 << 2
MOD_Mod1 = 1 << 3
MOD_Mod2 = 1 << 4
MOD_Mod3 = 1 << 5
MOD_Mod4 = 1 << 6
MOD_Mod5 = 1 << 7


def _gen_input_mask(mask):
    """Generate input mask from bytemask"""
    return input_mask(
        shift=bool(mask & MOD_Shift),
        lock=bool(mask & MOD_Lock),
        control=bool(mask & MOD_Control),
        mod1=bool(mask & MOD_Mod1),
        mod2=bool(mask & MOD_Mod2),
        mod3=bool(mask & MOD_Mod3),
        mod4=bool(mask & MOD_Mod4),
        mod5=bool(mask & MOD_Mod5))


class XError(Exception):
    pass


class Xdo(object):
    def __init__(self, display=None):
        if display is None:
            display = os.environ.get('DISPLAY', '')
        display = display.encode('utf-8')
        self._xdo = _libxdo.xdo_new(display)

        def _handle_x_error(evt):
            # todo: handle errors in a nicer way, eg. try getting error message
            raise XError('Event: {}'.format(evt))

        self._error_handler = XErrorHandler(_handle_x_error)

        _libX11.XSetErrorHandler(self._error_handler)

    @classmethod
    def version(cls):
        return _libxdo.xdo_version()

    @classmethod
    def version_info(cls):
        return tuple(int(x) for x in cls.version().split(b'.'))

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
        # screen = 0

        x = ctypes.c_int(x)
        y = ctypes.c_int(y)
        screen = ctypes.c_int(screen)

        _libxdo.xdo_move_mouse(self._xdo, x, y, screen)

    def move_mouse_relative_to_window(self, window, x, y):
        """
        Move the mouse to a specific location relative to the top-left corner
        of a window.

        :param x: the target X coordinate on the screen in pixels.
        :param y: the target Y coordinate on the screen in pixels.
        """
        _libxdo.xdo_move_mouse_relative_to_window(
            self._xdo, ctypes.c_ulong(window), x, y)

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
        _libxdo.xdo_mouse_down(
            self._xdo, ctypes.c_ulong(window), ctypes.c_int(button))

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
        _libxdo.xdo_mouse_up(
            self._xdo, ctypes.c_ulong(window), ctypes.c_int(button))

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
            self._xdo, window, keysequence, ctypes.c_ulong(delay))

    def send_keysequence_window_down(self, window, keysequence, delay=12000):
        """Send key press (down) events for the given key sequence"""
        _libxdo.xdo_send_keysequence_window_down(
            self._xdo, window, keysequence, ctypes.c_ulong(delay))

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

    def get_active_keys_to_keycode_list(self):
        """Get a list of active keys. Uses XQueryKeymap"""

        try:
            _libxdo.xdo_get_active_keys_to_keycode_list
        except AttributeError:
            # Apparently, this was implemented in a later version..
            raise NotImplementedError()

        keys = POINTER(charcodemap_t)
        nkeys = ctypes.c_int(0)
        _libxdo.xdo_get_active_keys_to_keycode_list(
            self._xdo, ctypes.byref(keys), ctypes.byref(nkeys))

        # todo: make sure this returns a list of charcodemap_t!
        return keys.value

    def wait_for_window_map_state(self, window, state):
        """
        Wait for a window to have a specific map state.

        State possibilities:
          IsUnmapped - window is not displayed.
          IsViewable - window is mapped and shown (though may be
              clipped by windows on top of it)
          IsUnviewable - window is mapped but a parent window is unmapped.

        :param window: the window you want to wait for.
        :param state: the state to wait for.
        """
        _libxdo.xdo_wait_for_window_map_state(self._xdo, window, state)

    def wait_for_window_size(self, window, width, height, flags, to_or_from):
        _libxdo.xdo_wait_for_window_size(self._xdo)

    def wait_for_window_size_to(self, window, width, height, flags=0):
        return self.wait_for_window_size(window, width, height, flags, 0)

    def wait_for_window_size_from(self, window, width, height, flags=0):
        return self.wait_for_window_size(window, width, height, flags, 1)

    def move_window(self, window, x, y):
        """
        Move a window to a specific location.

        The top left corner of the window will be moved to the x,y coordinate.

        :param wid: the window to move
        :param x: the X coordinate to move to.
        :param y: the Y coordinate to move to.
        """
        _libxdo.xdo_move_window(self._xdo, window, x, y)

    def translate_window_with_sizehint(self, window, width, height):
        """
        Apply a window's sizing hints (if any) to a given width and height.

        This function wraps XGetWMNormalHints() and applies any
        resize increment and base size to your given width and height values.

        :param window: the window to use
        :param width: the unit width you want to translate
        :param height: the unit height you want to translate
        :return: (width, height)
        """
        width_ret = ctypes.c_uint(0)
        height_ret = ctypes.c_uint(0)
        _libxdo.xdo_translate_window_with_sizehint(
            self._xdo, window, width, height,
            ctypes.byref(width_ret),
            ctypes.byref(height_ret))
        return width_ret.value, height_ret.value

    def set_window_size(self, window, w, h, flags=0):
        """
        Change the window size.

        :param wid: the window to resize
        :param w: the new desired width
        :param h: the new desired height
        :param flags: if 0, use pixels for units. If SIZE_USEHINTS, then
            the units will be relative to the window size hints.
        """
        _libxdo.xdo_set_window_size(self._xdo, window, w, h, flags)

    def set_window_property(self, window, name, value):
        """
        Change a window property.

        Example properties you can change are WM_NAME, WM_ICON_NAME, etc.

        :param wid: The window to change a property of.
        :param name: the string name of the property.
        :param value: the string value of the property.
        """
        _libxdo.xdo_set_window_property(self._xdo, window, name, value)

    def set_window_class(self, window, name, class_):
        """
        Change the window's classname and or class.

        :param name: The new class name. If ``None``, no change.
        :param class_: The new class. If ``None``, no change.
        """
        _libxdo.xdo_set_window_class(self._xdo, window, name, class_)

    def set_window_urgency(self, window, urgency):
        """Sets the urgency hint for a window"""
        _libxdo.xdo_set_window_urgency(self._xdo, window, urgency)

    def set_window_override_redirect(self, window, override_redirect):
        """
        Set the override_redirect value for a window. This generally means
        whether or not a window manager will manage this window.

        If you set it to 1, the window manager will usually not draw
        borders on the window, etc. If you set it to 0, the window manager
         will see it like a normal application window.
        """
        _libxdo.xdo_set_window_override_redirect(
            self._xdo, window, override_redirect)

    def focus_window(self, window):
        """
        Focus a window.

        :see: xdo_activate_window
        :param wid: the window to focus.
        """
        _libxdo.xdo_focus_window(self._xdo, window)

    def raise_window(self, window):
        """
        Raise a window to the top of the window stack. This is also sometimes
        termed as bringing the window forward.

        :param wid: The window to raise.
        """
        _libxdo.xdo_raise_window(self._xdo, window)

    def get_focused_window(self):
        """
        Get the window currently having focus.

        :param window_ret:
        Pointer to a window where the currently-focused window
        will be stored.
        """
        window_ret = window_t(0)
        _libxdo.xdo_get_focused_window(self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def wait_for_window_focus(self, window, want_focus):
        """
        Wait for a window to have or lose focus.

        :param window: The window to wait on
        :param want_focus: If 1, wait for focus. If 0, wait for loss of focus.
        """
        _libxdo.xdo_wait_for_window_focus(self._xdo, window, want_focus)

    def get_pid_window(self, window):
        """
        Get the PID owning a window. Not all applications support this.
        It looks at the ``_NET_WM_PID`` property of the window.

        :param window: the window to query.
        :return: the process id or 0 if no pid found.
        """
        # todo: if the pid is 0, it means "not found" -> exception?
        return _libxdo.xdo_get_pid_window(self._xdo, window)

    def get_focused_window_sane(self):
        """
        Like xdo_get_focused_window, but return the first ancestor-or-self
        window * having a property of WM_CLASS. This allows you to get
        the "real" or top-level-ish window having focus rather than something
        you may not expect to be the window having focused.

        :param window_ret:
            Pointer to a window where the currently-focused window
            will be stored.
        """
        window_ret = window_t(0)
        _libxdo.xdo_get_focused_window_sane(
            self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def activate_window(self, window):
        """
        Activate a window. This is generally a better choice than
        xdo_focus_window for a variety of reasons, but it requires window
        manager support:

        - If the window is on another desktop, that desktop is switched to.
        - It moves the window forward rather than simply focusing it

        Requires your window manager to support this.
        Uses _NET_ACTIVE_WINDOW from the EWMH spec.

        :param wid: the window to activate
        """
        _libxdo.xdo_activate_window(self._xdo, window)

    def wait_for_window_active(self, window, active=1):
        """
        Wait for a window to be active or not active.

        Requires your window manager to support this.
        Uses _NET_ACTIVE_WINDOW from the EWMH spec.

        :param window: the window to wait on
        :param active: If 1, wait for active. If 0, wait for inactive.
        """
        _libxdo.xdo_wait_for_window_active(self._xdo, window, active)

    def map_window(self, window):
        """
        Map a window. This mostly means to make the window visible if it is
        not currently mapped.

        :param wid: the window to map.
        """
        _libxdo.xdo_map_window(self._xdo, window)

    def unmap_window(self, window):
        """
        Unmap a window

        :param wid: the window to unmap
        """
        _libxdo.xdo_unmap_window(self._xdo, window)

    def minimize_window(self, window):
        """Minimize a window"""
        _libxdo.xdo_minimize_window(self._xdo, window)

    def reparent_window(self, window_source, window_target):
        """
        Reparents a window

        :param wid_source: the window to reparent
        :param wid_target: the new parent window
        """
        _libxdo.xdo_reparent_window(self._xdo, window_source, window_target)

    def get_window_location(self, window):
        """
        Get a window's location.
        """
        screen_ret = Screen()
        x_ret = ctypes.c_int(0)
        y_ret = ctypes.c_int(0)
        _libxdo.xdo_get_window_location(
            self._xdo, window, ctypes.byref(x_ret), ctypes.byref(y_ret),
            ctypes.byref(screen_ret))
        return window_location(x_ret.value, y_ret.value, screen_ret)

    def get_window_size(self, window):
        """
        Get a window's size.
        """
        w_ret = ctypes.c_uint(0)
        h_ret = ctypes.c_uint(0)
        _libxdo.xdo_get_window_size(self._xdo, window, ctypes.byref(w_ret),
                                    ctypes.byref(h_ret))
        return window_size(w_ret.value, h_ret.value)

    def get_active_window(self):
        """
        Get the currently-active window.
        Requires your window manager to support this.
        Uses ``_NET_ACTIVE_WINDOW`` from the EWMH spec.
        """
        window_ret = window_t(0)
        _libxdo.xdo_get_active_window(self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def select_window_with_click(self):
        """
        Get a window ID by clicking on it.
        This function blocks until a selection is made.
        """
        window_ret = window_t(0)
        _libxdo.xdo_select_window_with_click(
            self._xdo, ctypes.byref(window_ret))
        return window_ret.value

    def set_number_of_desktops(self, ndesktops):
        """
        Set the number of desktops.
        Uses ``_NET_NUMBER_OF_DESKTOPS`` of the EWMH spec.

        :param ndesktops: the new number of desktops to set.
        """
        _libxdo.xdo_set_number_of_desktops(self._xdo, ndesktops)

    def get_number_of_desktops(self):
        """
        Get the current number of desktops.
        Uses ``_NET_NUMBER_OF_DESKTOPS`` of the EWMH spec.

        :param ndesktops:
            pointer to long where the current number of desktops is stored
        """
        ndesktops = ctypes.c_long(0)
        _libxdo.xdo_get_number_of_desktops(self._xdo, ctypes.byref(ndesktops))
        return ndesktops.value

    def set_current_desktop(self, desktop):
        """
        Switch to another desktop.
        Uses ``_NET_CURRENT_DESKTOP`` of the EWMH spec.

        :param desktop: The desktop number to switch to.
        """
        _libxdo.xdo_set_current_desktop(self._xdo, desktop)

    def get_current_desktop(self):
        """
        Get the current desktop.
        Uses ``_NET_CURRENT_DESKTOP`` of the EWMH spec.
        """
        desktop = ctypes.c_long(0)
        _libxdo.xdo_get_current_desktop(self._xdo, ctypes.byref(desktop))
        return desktop.value

    def set_desktop_for_window(self, window, desktop):
        """
        Move a window to another desktop
        Uses _NET_WM_DESKTOP of the EWMH spec.

        :param wid: the window to move
        :param desktop: the desktop destination for the window
        """
        _libxdo.xdo_set_desktop_for_window(self._xdo, window, desktop)

    def get_desktop_for_window(self, window):
        """
        Get the desktop a window is on.
        Uses _NET_WM_DESKTOP of the EWMH spec.

        If your desktop does not support ``_NET_WM_DESKTOP``, then '*desktop'
        remains unmodified.

        :param wid: the window to query
        """
        desktop = ctypes.c_long(0)
        _libxdo.xdo_get_desktop_for_window(
            self._xdo, window, ctypes.byref(desktop))
        return desktop.value

    def search_windows(
            self, winname=None, winclass=None, winclassname=None,
            pid=None, only_visible=False, screen=None, require=False,
            searchmask=0, desktop=None, limit=0, max_depth=-1):
        """
        Search for windows.

        :param winname:
            Regexp to be matched against window name
        :param winclass:
            Regexp to be matched against window class
        :param winclassname:
            Regexp to be matched against window class name
        :param pid:
            Only return windows from this PID
        :param only_visible:
            If True, only return visible windows
        :param screen:
            Search only windows on this screen
        :param require:
            If True, will match ALL conditions. Otherwise, windows matching
            ANY condition will be returned.
        :param searchmask:
            Search mask, for advanced usage. Leave this alone if you
            don't kwnow what you are doing.
        :param limit:
            Maximum number of windows to list. Zero means no limit.
        :param max_depth:
            Maximum depth to return. Defaults to -1, meaning "no limit".
        :return:
            A list of window ids matching query.
        """
        windowlist_ret = ctypes.pointer(window_t(0))
        nwindows_ret = ctypes.c_uint(0)

        search = xdo_search_t(searchmask=searchmask)

        if winname is not None:
            search.winname = winname
            search.searchmask |= SEARCH_NAME

        if winclass is not None:
            search.winclass = winclass
            search.searchmask |= SEARCH_CLASS

        if winclassname is not None:
            search.winclassname = winclassname
            search.searchmask |= SEARCH_CLASSNAME

        if pid is not None:
            search.pid = pid
            search.searchmask |= SEARCH_PID

        if only_visible:
            search.only_visible = True
            search.searchmask |= SEARCH_ONLYVISIBLE

        if screen is not None:
            search.screen = screen
            search.searchmask |= SEARCH_SCREEN

        if screen is not None:
            search.screen = desktop
            search.searchmask |= SEARCH_DESKTOP

        search.limit = limit
        search.max_depth = max_depth

        _libxdo.xdo_search_windows(
            self._xdo, search,
            ctypes.byref(windowlist_ret),
            ctypes.byref(nwindows_ret))

        return [windowlist_ret[i] for i in range(nwindows_ret.value)]

    def get_window_property_by_atom(self, window, atom):
        # todo: figure out what exactly this method does, and implement it
        raise NotImplemented(
            "get_window_property_by_atom() is not implemented (yet)")

    def get_window_property(self, window, name):
        value = ctypes.c_char_p()  # unsigned char **value
        nitems = ctypes.c_long()
        type_ = Atom()
        size = ctypes.c_int(0)

        _libxdo.xdo_get_window_property(
            self._xdo, window, name, ctypes.byref(value), ctypes.byref(nitems),
            ctypes.byref(type_), ctypes.byref(size))

        # todo: we need to convert atoms into their actual type..
        values = []
        for i in range(nitems):
            i_val = value[i]
            # i_type = type_[i]
            values.append(i_val)
            # todo: perform type conversion for "Atom"s of this type?
            # todo: how does the "Atom" thing work?

        return values

    def get_input_state(self):
        """
        Get the current input state.

        :return:
            a namedtuple with the following (boolean) fields:
            shift, lock, control, mod1, mod2, mod3, mod4, mod5
        """
        mask = _libxdo.xdo_get_input_state(self._xdo)
        return _gen_input_mask(mask)

    def get_symbol_map(self):
        """
        If you need the symbol map, use this method.

        The symbol map is an array of string pairs mapping common tokens
        to X Keysym strings, such as "alt" to "Alt_L"

        :return: array of strings.
        """
        # todo: make sure we return a list of strings!
        sm = _libxdo.xdo_get_symbol_map()

        # Return value is like:
        # ['alt', 'Alt_L', ..., None, None, None, ...]
        # We want to return only values up to the first None.
        # todo: any better solution than this?
        i = 0
        ret = []
        while True:
            c = sm[i]
            if c is None:
                return ret
            ret.append(c)
            i += 1

    def get_active_modifiers(self):
        """
        Get a list of active keys. Uses XQueryKeymap.

        :return: list of charcodemap_t instances
        """
        keys = ctypes.pointer(charcodemap_t())
        nkeys = ctypes.c_int(0)

        _libxdo.xdo_get_active_modifiers(
            self._xdo, ctypes.byref(keys), ctypes.byref(nkeys))
        return [keys[i] for i in range(nkeys.value)]

    def clear_active_modifiers(self, window, mods=None):
        """
        Send any events necesary to clear the the active modifiers.
        For example, if you are holding 'alt' when xdo_get_active_modifiers is
        called, then this method will send a key-up for 'alt'
        """
        raise NotImplementedError()

    def set_active_modifiers(self, window, mods=None):
        """
        Send any events necessary to make these modifiers active.
        This is useful if you just cleared the active modifiers and then wish
        to restore them after.
        """
        raise NotImplementedError()

    def get_desktop_viewport(self):
        """
        Get the position of the current viewport.

        This is only relevant if your window manager supports
        ``_NET_DESKTOP_VIEWPORT``.
        """
        raise NotImplementedError()

    def set_desktop_viewport(self, x, y):
        """
        Set the position of the current viewport.

        This is only relevant if your window manager supports
        ``_NET_DESKTOP_VIEWPORT``
        """
        raise NotImplementedError()

    def kill_window(self):
        """
        Kill a window and the client owning it.
        """
        raise NotImplementedError()

    XDO_FIND_PARENTS = 0
    XDO_FIND_CHILDREN = 1

    def find_window_client(self):
        """
        Find a client window (child) in a given window. Useful if you get the
        window manager's decorator window rather than the client window.
        """
        raise NotImplementedError()

    def get_window_name(self, win_id):
        """
        Get a window's name, if any.
        """
        window = window_t(win_id)
        name_ptr = ctypes.c_char_p()
        name_len = ctypes.c_int(0)
        name_type = ctypes.c_int(0)
        _libxdo.xdo_get_window_name(
            self._xdo, window, ctypes.byref(name_ptr),
            ctypes.byref(name_len), ctypes.byref(name_type))
        name = name_ptr.value
        _libX11.XFree(name_ptr)  # Free the string allocated by Xlib
        return name

    def enable_feature(self):
        """
        Enable an xdo feature.

        This function is mainly used by libxdo itself, however,
        you may find it useful in your own applications.

        :see: XDO_FEATURES
        """
        raise NotImplementedError()

    def has_feature(self):
        """
        Check if a feature is enabled.

        This function is mainly used by libxdo itself, however,
        you may find it useful in your own applications.

        :see: XDO_FEATURES
        """
        raise NotImplementedError()

    def get_viewport_dimensions(self):
        """
        Query the viewport (your display) dimensions

        If Xinerama is active and supported, that api internally is used.
        If Xineram is disabled, we will report the root window's dimensions
        for the given screen.
        """
        raise NotImplementedError()

    def __del__(self):
        _libxdo.xdo_free(self._xdo)
