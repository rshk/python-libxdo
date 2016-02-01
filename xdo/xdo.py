# -*- coding: utf-8 -*-

"""
Ctypes bindings for libxdo
"""

import ctypes
from ctypes import c_uint8 as c_uchar
from ctypes import (
    POINTER, Structure, c_bool, c_char, c_char_p, c_int, c_long, c_uint,
    c_ulong, c_void_p, c_wchar)

from ctypes.util import find_library

libxdo = ctypes.CDLL(find_library("xdo"))
libX11 = ctypes.CDLL(find_library("X11"))  # Import XFree

XDO_ERROR = 1
XDO_SUCCESS = 0

# Window type is just defined as ``unsigned long``
window_t = c_ulong
useconds_t = c_ulong
# screen_t = c_ulong  # warning! this is simply guessed!

XID = c_ulong
Colormap = XID
GContext = XID
Pixmap = XID
Font = XID


class XdoException(Exception):
    pass


def _errcheck(result, func, arguments):
    """
    Error checker for functions returning an integer indicating
    success (0) / failure (1).

    Raises a XdoException in case of error, otherwise just
    returns ``None`` (returning the original code, 0, would be
    useless anyways..)
    """

    if result != 0:
        raise XdoException(
            'Function {0} returned error code {1}'
            .format(func.__name__, result))
    return None  # be explicit :)


# Keep reference of libraries from which stuff is taken..
# #include <sys/types.h>
# #include <X11/Xlib.h>
# #include <X11/X.h>
# #include <unistd.h>
# #include <wchar.h>

# When issuing a window size change, giving this flag will make the size
# change be relative to the size hints of the window.  For terminals, this
# generally means that the window size will be relative to the font size,
# allowing you to change window sizes based on character rows and columns
# instead of pixels.

SIZE_USEHINTS = 1 << 0
SIZE_USEHINTS_X = 1 << 1
SIZE_USEHINTS_Y = 1 << 2

# CURRENTWINDOW is a special identify for xdo input faking (mouse and
# keyboard) functions like xdo_send_keysequence_window that indicate
# we should target the current window, not a specific window.
#
# Generally, this means we will use XTEST instead of XSendEvent when sending
# events.

CURRENTWINDOW = ctypes.c_ulong(0)


KeyCode = c_char  # unsinged char
KeySym = c_ulong  # XID -> unsigned long


class charcodemap_t(Structure):
    """
    Map character to whatever information we need to be able to send
    this key (keycode, modifiers, group, etc)
    """

    _fields_ = [
        # wchar_t key; // the letter for this key, like 'a'
        ('key', c_wchar),

        # KeyCode code; // the keycode that this key is on
        ('code', KeyCode),

        # KeySym symbol; // the symbol representing this key
        ('symbol', KeySym),

        # int group; // the keyboard group that has this key in it
        ('group', c_int),

        # int modmask; // the modifiers to apply when sending this key
        #              // if this key need to be bound at runtime because
        #              // it does not exist in the current keymap, this will
        #              // be set to 1.
        ('modmask', c_int),

        # int needs_binding;
        ('needs_binding', c_int),
    ]


# typedef enum {
#   XDO_FEATURE_XTEST, /** Is XTest available? */
# } XDO_FEATURES;
XDO_FEATURE_XTEST = 0


class xdo_t(Structure):
    """The main context"""

    _fields_ = [
        # The Display for Xlib
        # Display *xdpy;
        ('xdpy', c_void_p),

        # The display name, if any. NULL if not specified.
        #   char *display_name;
        ('display_name', c_char_p),

        # @internal Array of known keys/characters
        #   charcodemap_t *charcodes;
        ('charcodes', POINTER(charcodemap_t)),

        # @internal Lenth of charcodes array
        #   int charcodes_len;
        ('charcodes_len', c_int),

        # @internal highest keycode value highest and lowest keycodes
        #   int keycode_high;
        ('keycode_high', c_int),

        # @internal lowest keycode value used by this X server
        #   int keycode_low;
        ('keycode_low', c_int),

        # @internal number of keysyms per keycode
        #   int keysyms_per_keycode;
        ('keysyms_per_keycode', c_int),

        # Should we close the display when calling xdo_free?
        #   int close_display_when_freed;
        ('close_display_when_freed', c_int),

        # Be extra quiet? (omits some error/message output)
        #   int quiet;
        ('quiet', c_int),

        # Enable debug output?
        #   int debug;
        ('debug', c_int),

        # Feature flags, such as XDO_FEATURE_XTEST, etc...
        #   int features_mask;
        ('features_mask', c_int),
    ]


class XErrorEvent(Structure):
    _fields = [
        # int type;
        ('type', c_int),
        # Display *display;       /* Display the event was read from */
        ('display', c_void_p),
        # XID resourceid;         /* resource id */
        ('resourceid', XID),
        # unsigned long serial;   /* serial number of failed request */
        ('serial', c_ulong),
        # unsigned char error_code;       /* error code of failed request */
        ('error_code', c_uchar),
        # unsigned char request_code;     /* Major op-code of failed request */
        ('request_code', c_uchar),
        # unsigned char minor_code;       /* Minor op-code of failed request */
        ('minor_code', c_uchar),
    ]


# Search only window title. DEPRECATED - Use SEARCH_NAME
SEARCH_TITLE = 1 << 0

# Search only window class.
SEARCH_CLASS = 1 << 1

# Search only window name.
SEARCH_NAME = 1 << 2

# Search only window pid.
SEARCH_PID = 1 << 3

# Search only visible windows.
SEARCH_ONLYVISIBLE = 1 << 4

# Search only a specific screen.
SEARCH_SCREEN = 1 << 5

# Search only window class name.
SEARCH_CLASSNAME = 1 << 6

# Search a specific desktop
SEARCH_DESKTOP = 1 << 7


SEARCH_ANY = 0
SEARCH_ALL = 1


class xdo_search_t(Structure):
    """
    The window search query structure.

    :see: xdo_search_windows
    """

    _fields_ = [
        # const char *title; pattern to test against a window title
        ('title', c_char_p),

        # const char *winclass; pattern to test against a window class
        ('winclass', c_char_p),

        # const char *winclassname; pattern to test against a window class
        ('winclassname', c_char_p),

        # const char *winname; pattern to test against a window name
        ('winname', c_char_p),

        # int pid; window pid (From window atom _NET_WM_PID)
        ('pid', c_int),

        # long max_depth; depth of search. 1 means only toplevel windows
        ('max_depth', c_long),

        # int only_visible; boolean; set true to search only visible windows
        ('only_visible', c_int),

        # int screen; what screen to search, if any. If none given,
        #             search all screens
        ('screen', c_int),

        # Should the tests be 'and' or 'or' ? If 'and', any failure
        # will skip the window. If 'or', any success will keep the window
        # in search results.
        # enum { SEARCH_ANY, SEARCH_ALL } require;
        ('require', c_int),

        # bitmask of things you are searching for, such as SEARCH_NAME, etc.
        # :see: SEARCH_NAME, SEARCH_CLASS, SEARCH_PID, SEARCH_CLASSNAME, etc
        # unsigned int searchmask;
        ('searchmask', c_uint),

        # What desktop to search, if any. If none given, search
        # all screens.
        # long desktop;
        ('desktop', c_long),

        # How many results to return? If 0, return all.
        # unsigned int limit;
        ('limit', c_uint),
    ]


# Defined in X11/Xlib.h
class XGCValues(Structure):
    _fields_ = [
        ('function', c_int),  # logical operation
        ('plane_mask', c_ulong),  # plane mask
        ('foreground', c_ulong),  # foreground pixel
        ('background', c_ulong),  # background pixel
        ('line_width', c_int),  # line width
        ('line_style', c_int),  # LineSolid, LineOnOffDash, LineDoubleDash
        ('cap_style', c_int),  # CapNotLast, CapButt, CapRound, CapProjecting
        ('join_style', c_int),  # JoinMiter, JoinRound, JoinBevel
        ('fill_style', c_int),  # FillSolid, FillTiled, FillStippled, FillOpaeueStippled  # noqa
        ('fill_rule', c_int),  # EvenOddRule, WindingRule
        ('arc_mode', c_int),  # ArcChord, ArcPieSlice
        ('tile', Pixmap),  # tile pixmap for tiling operations
        ('stipple', Pixmap),  # stipple 1 plane pixmap for stipping
        ('ts_x_origin', c_int),  # offset for tile or stipple operations
        ('ts_y_origin', c_int),
        ('font', Font),  # default text font for text operations
        ('subwindow_mode', c_int),  # ClipByChildren, IncludeInferiors
        ('graphics_exposures', c_bool),  # boolean, should exposures be generated # noqa
        ('clip_x_origin', c_int),  # origin for clipping
        ('clip_y_origin', c_int),
        ('clip_mask', Pixmap),  # bitmap clipping; other calls for rects */
        ('dash_offset', c_int),  # patterned/dashed line information
        ('dashes', c_char),
    ]


# Defined in X11/Xlibint.h as _XGC -> then typedef'd as "GC"
class _XGC(Structure):
    _fields_ = [
        ('ext_data', c_void_p),  # (XExtData *) hook for extension to hang data
        ('gid', GContext),  # protocol ID for graphics context
        ('reacts', c_bool),  # boolean: TRUE if clipmask is list of rectangles
        ('dashes', c_bool),  # boolean: TRUE if dash-list is really a list
        ('dirty', c_ulong),  # cache dirty bits
        ('values', XGCValues),  # shadow structure of values
    ]


# Screen is defined in X11/Xlib.h
class Screen(Structure):
    _fields_ = [
        # XExtData *ext_data;	/* hook for extension to hang data */
        ('ext_data', c_void_p),

        # struct _XDisplay *display;/* back pointer to display structure */
        ('display', c_void_p),

        # Window root;		/* Root window id. */
        ('root', window_t),

        # int width, height;	/* width and height of screen */
        ('width', c_int),
        ('height', c_int),

        # int mwidth, mheight;	/* width and height of  in millimeters */
        ('mwidth', c_int),
        ('mheight', c_int),

        # int ndepths;		/* number of depths possible */
        ('ndepths', c_int),

        # Depth *depths;		/* list of allowable depths on the screen */
        ('depths', c_void_p),

        # int root_depth;		/* bits per pixel */
        ('root_depth', c_int),

        # Visual *root_visual;	/* root visual */
        ('root_visual', c_void_p),

        # GC default_gc;		/* GC for the root root visual */
        ('default_gc', _XGC),

        # Colormap cmap;		/* default color map */
        ('cmap', Colormap),

        # unsigned long white_pixel;
        ('white_pixel', c_ulong),

        # unsigned long black_pixel;	/* White and Black pixel values */
        ('black_pixel', c_ulong),

        # int max_maps, min_maps;	/* max and min color maps */
        ('max_maps', c_int),
        ('min_maps', c_int),

        # int backing_store;	/* Never, WhenMapped, Always */
        ('backing_store', c_int),

        # Bool save_unders;
        ('save_unders', c_bool),

        # long root_input_mask;	/* initial root input mask */
        ('root_input_mask', c_long),
    ]


# From X11/Xdefs.h
# typedef unsigned long Atom;
Atom = atom_t = c_ulong

# ============================================================================
# xdo_t* xdo_new(const char *display);
libxdo.xdo_new.argtypes = (c_char_p,)
libxdo.xdo_new.restype = POINTER(xdo_t)
libxdo.xdo_new.__doc__ = """\
Create a new xdo_t instance.

:param display: the string display name, such as ":0". If null, uses the
environment variable DISPLAY just like XOpenDisplay(NULL).

:return: Pointer to a new xdo_t or NULL on failure
"""

# ============================================================================
# xdo_t* xdo_new_with_opened_display(Display *xdpy, const char *display,
#                                    int close_display_when_freed);
libxdo.xdo_new_with_opened_display.__doc__ = """\
Create a new xdo_t instance with an existing X11 Display instance.

:param xdpy: the Display pointer given by a previous XOpenDisplay()
:param display: the string display name
:param close_display_when_freed: If true, we will close the display when
    xdo_free is called. Otherwise, we leave it open.
"""

# ============================================================================
# const char *xdo_version(void);
libxdo.xdo_version.argtypes = ()
libxdo.xdo_version.restype = c_char_p
libxdo.xdo_version.__doc__ = """\
Return a string representing the version of this library
"""

# ============================================================================
# void xdo_free(xdo_t *xdo);
libxdo.xdo_free.argtypes = (POINTER(xdo_t),)
libxdo.xdo_free.__doc__ = """\
Free and destroy an xdo_t instance.

If close_display_when_freed is set, then we will also close the Display.
"""

# ============================================================================
# int xdo_move_mouse(const xdo_t *xdo, int x, int y, int screen);
libxdo.xdo_move_mouse.argtypes = (
    POINTER(xdo_t), c_int, c_int, c_int)
libxdo.xdo_move_mouse.restype = c_int
libxdo.xdo_move_mouse.errcheck = _errcheck
libxdo.xdo_move_mouse.__doc__ = """
Move the mouse to a specific location.

:param x: the target X coordinate on the screen in pixels.
:param y: the target Y coordinate on the screen in pixels.
:param screen: the screen (number) you want to move on.
"""

# ============================================================================
# int xdo_move_mouse_relative_to_window(const xdo_t *xdo,
#     Window window, int x, int y);
libxdo.xdo_move_mouse_relative_to_window.argtypes = (
    POINTER(xdo_t), POINTER(window_t), c_int, c_int)
libxdo.xdo_move_mouse_relative_to_window.restype = c_int
libxdo.xdo_move_mouse_relative_to_window.errcheck = _errcheck
libxdo.xdo_move_mouse_relative_to_window.__doc__ = """\
Move the mouse to a specific location relative to the top-left corner
of a window.

:param x: the target X coordinate on the screen in pixels.
:param y: the target Y coordinate on the screen in pixels.
"""

# ============================================================================
# int xdo_move_mouse_relative(const xdo_t *xdo, int x, int y);
libxdo.xdo_move_mouse_relative.argtypes = (POINTER(xdo_t), c_int, c_int)
libxdo.xdo_move_mouse_relative.restype = c_int
libxdo.xdo_move_mouse_relative.errcheck = _errcheck
libxdo.xdo_move_mouse_relative.__doc__ = """\
Move the mouse relative to it's current position.

:param x: the distance in pixels to move on the X axis.
:param y: the distance in pixels to move on the Y axis.
"""

# ============================================================================
# int xdo_mouse_down(const xdo_t *xdo, Window window, int button);
libxdo.xdo_mouse_down.argtypes = (POINTER(xdo_t), window_t, c_int)
libxdo.xdo_mouse_down.restype = c_int
libxdo.xdo_mouse_down.errcheck = _errcheck
libxdo.xdo_mouse_down.__doc__ = """\
Send a mouse press (aka mouse down) for a given button at the current mouse
location.

:param window: The window you want to send the event to or CURRENTWINDOW
:param button: The mouse button. Generally, 1 is left, 2 is middle, 3 is
   right, 4 is wheel up, 5 is wheel down.
"""

# ============================================================================
# int xdo_mouse_up(const xdo_t *xdo, Window window, int button);
libxdo.xdo_mouse_up.argtypes = (POINTER(xdo_t), window_t, c_int)
libxdo.xdo_mouse_up.restype = c_int
libxdo.xdo_mouse_up.errcheck = _errcheck
libxdo.xdo_mouse_up.__doc__ = """\
Send a mouse release (aka mouse up) for a given button at the current mouse
location.

:param window: The window you want to send the event to or CURRENTWINDOW
:param button: The mouse button. Generally, 1 is left, 2 is middle, 3 is
   right, 4 is wheel up, 5 is wheel down.
"""

# ============================================================================
# int xdo_get_mouse_location(const xdo_t *xdo, int *x, int *y,
#                            int *screen_num);
libxdo.xdo_get_mouse_location.argtypes = (POINTER(xdo_t), POINTER(c_int),
                                          POINTER(c_int), POINTER(c_int))
libxdo.xdo_get_mouse_location.restype = c_int
libxdo.xdo_get_mouse_location.errcheck = _errcheck
libxdo.xdo_get_mouse_location.__doc__ = """\
Get the current mouse location (coordinates and screen number).

:param x: integer pointer where the X coordinate will be stored
:param y: integer pointer where the Y coordinate will be stored
:param screen_num: integer pointer where the screen number will be stored
"""

# ============================================================================
# int xdo_get_window_at_mouse(const xdo_t *xdo, Window *window_ret);
libxdo.xdo_get_window_at_mouse.argtypes = (POINTER(xdo_t), POINTER(window_t))
libxdo.xdo_get_window_at_mouse.restype = c_int
libxdo.xdo_get_window_at_mouse.errcheck = _errcheck
libxdo.xdo_get_window_at_mouse.__doc__ = """\
Get the window the mouse is currently over

:param window_ret: Winter pointer where the window will be stored.
"""

# ============================================================================
# int xdo_get_mouse_location2(const xdo_t *xdo, int *x_ret, int *y_ret,
#                             int *screen_num_ret, Window *window_ret);
libxdo.xdo_get_mouse_location2.argtypes = (
    POINTER(xdo_t), POINTER(c_int), POINTER(c_int),
    POINTER(c_int), POINTER(window_t))
libxdo.xdo_get_mouse_location2.restype = c_int
libxdo.xdo_get_mouse_location2.errcheck = _errcheck
libxdo.xdo_get_mouse_location2.__doc__ = """\
Get all mouse location-related data.

If null is passed for any parameter, we simply do not store it.
Useful if you only want the 'y' coordinate, for example.

:param x: integer pointer where the X coordinate will be stored
:param y: integer pointer where the Y coordinate will be stored
:param screen_num: integer pointer where the screen number will be stored
:param window: Window pointer where the window/client the mouse is over
  will be stored.
"""

# ============================================================================
# int xdo_wait_for_mouse_move_from(const xdo_t *xdo, int origin_x,
#                                  int origin_y);
libxdo.xdo_wait_for_mouse_move_from.argtypes = (POINTER(xdo_t), c_int, c_int)
libxdo.xdo_wait_for_mouse_move_from.restype = c_int
libxdo.xdo_wait_for_mouse_move_from.errcheck = _errcheck
libxdo.xdo_wait_for_mouse_move_from.__doc__ = """
Wait for the mouse to move from a location. This function will block
until the condition has been satisified.

:param origin_x: the X position you expect the mouse to move from
:param origin_y: the Y position you expect the mouse to move from
"""

# ============================================================================
# int xdo_wait_for_mouse_move_to(const xdo_t *xdo, int dest_x, int dest_y);
libxdo.xdo_wait_for_mouse_move_to.argtypes = (POINTER(xdo_t), c_int, c_int)
libxdo.xdo_wait_for_mouse_move_to.restype = c_int
libxdo.xdo_wait_for_mouse_move_to.errcheck = _errcheck
libxdo.xdo_wait_for_mouse_move_to.__doc__ = """\
Wait for the mouse to move to a location. This function will block
until the condition has been satisfied.

:param dest_x: the X position you expect the mouse to move to
:param dest_y: the Y position you expect the mouse to move to
"""

# ============================================================================
# int xdo_click_window(const xdo_t *xdo, Window window, int button);
libxdo.xdo_click_window.argtypes = (POINTER(xdo_t), window_t, c_int)
libxdo.xdo_click_window.restype = c_int
libxdo.xdo_click_window.errcheck = _errcheck
libxdo.xdo_click_window.__doc__ = """\
Send a click for a specific mouse button at the current mouse location.

:param window: The window you want to send the event to or CURRENTWINDOW
:param button: The mouse button. Generally, 1 is left, 2 is middle, 3 is
   right, 4 is wheel up, 5 is wheel down.
"""

# ============================================================================
# int xdo_click_window_multiple(const xdo_t *xdo, Window window, int button,
#                        int repeat, useconds_t delay);
libxdo.xdo_click_window_multiple.argtypes = (
    POINTER(xdo_t), window_t, c_int, c_int, useconds_t)
libxdo.xdo_click_window_multiple.restype = c_int
libxdo.xdo_click_window_multiple.errcheck = _errcheck
libxdo.xdo_click_window_multiple.__doc__ = """\
Send a one or more clicks for a specific mouse button at the current mouse
location.

:param window: The window you want to send the event to or CURRENTWINDOW
:param button: The mouse button. Generally, 1 is left, 2 is middle, 3 is
   right, 4 is wheel up, 5 is wheel down.
"""

# ============================================================================
# int xdo_enter_text_window(const xdo_t *xdo, Window window,
#     const char *string, useconds_t delay);
libxdo.xdo_enter_text_window.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, useconds_t)
libxdo.xdo_enter_text_window.restype = c_int
libxdo.xdo_enter_text_window.errcheck = _errcheck
libxdo.xdo_enter_text_window.__doc__ = """
Type a string to the specified window.

If you want to send a specific key or key sequence, such as "alt+l", you
want instead xdo_send_keysequence_window(...).

:param window: The window you want to send keystrokes to or CURRENTWINDOW
:param string: The string to type, like "Hello world!"
:param delay: The delay between keystrokes in microseconds.
    12000 is a decent choice if you don't have other plans.
"""

# ============================================================================
# int xdo_send_keysequence_window(const xdo_t *xdo, Window window,
#                     const char *keysequence, useconds_t delay);
libxdo.xdo_send_keysequence_window.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, useconds_t)
libxdo.xdo_send_keysequence_window.restype = c_int
libxdo.xdo_send_keysequence_window.errcheck = _errcheck
libxdo.xdo_send_keysequence_window.__doc__ = """\
Send a keysequence to the specified window.

This allows you to send keysequences by symbol name. Any combination
of X11 KeySym names separated by '+' are valid. Single KeySym names
are valid, too.

Examples:
  "l"
  "semicolon"
  "alt+Return"
  "Alt_L+Tab"

If you want to type a string, such as "Hello world." you want to instead
use xdo_enter_text_window.

:param window: The window you want to send the keysequence to or
  CURRENTWINDOW
:param keysequence: The string keysequence to send.
:param delay: The delay between keystrokes in microseconds.
"""

# ============================================================================
# int xdo_send_keysequence_window_up(const xdo_t *xdo, Window window,
#                        const char *keysequence, useconds_t delay);
libxdo.xdo_send_keysequence_window_up.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, useconds_t)
libxdo.xdo_send_keysequence_window_up.restype = c_int
libxdo.xdo_send_keysequence_window_up.errcheck = _errcheck
libxdo.xdo_send_keysequence_window_up.__doc__ = """\
Send key release (up) events for the given key sequence.

:see: xdo_send_keysequence_window
"""

# ============================================================================
# int xdo_send_keysequence_window_down(const xdo_t *xdo, Window window,
#                          const char *keysequence, useconds_t delay);
libxdo.xdo_send_keysequence_window_down.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, useconds_t)
libxdo.xdo_send_keysequence_window_down.restype = c_int
libxdo.xdo_send_keysequence_window_down.errcheck = _errcheck
libxdo.xdo_send_keysequence_window_down.__doc__ = """\
Send key press (down) events for the given key sequence.

:see: xdo_send_keysequence_window
"""

# ============================================================================
# int xdo_send_keysequence_window_list_do(const xdo_t *xdo, Window window,
#                             charcodemap_t *keys, int nkeys,
#                             int pressed, int *modifier, useconds_t delay);
libxdo.xdo_send_keysequence_window_list_do.argtypes = (
    POINTER(xdo_t), window_t, POINTER(charcodemap_t), c_int, c_int,
    POINTER(c_int), useconds_t)
libxdo.xdo_send_keysequence_window_list_do.restype = c_int
libxdo.xdo_send_keysequence_window_list_do.errcheck = _errcheck
libxdo.xdo_send_keysequence_window_list_do.__doc__ = """\
Send a series of keystrokes.

:param window: The window to send events to or CURRENTWINDOW
:param keys: The array of charcodemap_t entities to send.
:param nkeys: The length of the keys parameter
:param pressed: 1 for key press, 0 for key release.
:param modifier: Pointer to integer to record the modifiers activated by
  the keys being pressed. If NULL, we don't save the modifiers.
:param delay: The delay between keystrokes in microseconds.
"""

# ============================================================================
# int xdo_get_active_keys_to_keycode_list(
#     const xdo_t *xdo, charcodemap_t **keys, int *nkeys);

# --- Note -------------------------------------------------------------
# This seems to be missing in version 3.20140213.1 (Debian Jessie)
# but is there in current Git master.
# Let's configure it only if it is available (meaning we are using
# a recent version of the library)

try:
    libxdo.xdo_get_active_keys_to_keycode_list.argtypes = (
        POINTER(xdo_t), POINTER(POINTER(charcodemap_t)), POINTER(c_int))
    libxdo.xdo_get_active_keys_to_keycode_list.restype = c_int
    libxdo.xdo_get_active_keys_to_keycode_list.errcheck = _errcheck
    libxdo.xdo_get_active_keys_to_keycode_list.__doc__ = """\
    Get a list of active keys. Uses XQueryKeymap.

    :param keys: Pointer to the array of charcodemap_t that will be allocated
       by this function.
    :param nkeys: Pointer to integer where the number of keys will be stored.
    """
except AttributeError:
    pass

# ============================================================================
# int xdo_wait_for_window_map_state(const xdo_t *xdo, Window wid,
#     int map_state);
libxdo.xdo_wait_for_window_map_state.argtypes = (
    POINTER(xdo_t), window_t, c_int)
libxdo.xdo_wait_for_window_map_state.restype
libxdo.xdo_wait_for_window_map_state.errcheck = _errcheck
libxdo.xdo_wait_for_window_map_state.__doc__ = """\
Wait for a window to have a specific map state.

State possibilities:
  IsUnmapped - window is not displayed.
  IsViewable - window is mapped and shown (though may be clipped by windows
    on top of it)
  IsUnviewable - window is mapped but a parent window is unmapped.

:param wid: the window you want to wait for.
:param map_state: the state to wait for.
"""

# ============================================================================
# Constants for xdo_wait_for_window_size()
SIZE_TO = 0
SIZE_FROM = 1

# ============================================================================
# int xdo_wait_for_window_size(
#     const xdo_t *xdo, Window window, unsigned int width,
#     unsigned int height, int flags, int to_or_from);
libxdo.xdo_wait_for_window_size.argtypes = (
    POINTER(xdo_t), window_t, c_uint, c_uint, c_int, c_int)
libxdo.xdo_wait_for_window_size.restype = c_int
libxdo.xdo_wait_for_window_size.errcheck = _errcheck
libxdo.xdo_wait_for_window_size.__doc__ = """\
"""


# ============================================================================
# int xdo_move_window(const xdo_t *xdo, Window wid, int x, int y);
libxdo.xdo_move_window.argtypes = (
    POINTER(xdo_t), window_t, c_int, c_int)
libxdo.xdo_move_window.restype = c_int
libxdo.xdo_move_window.errcheck = _errcheck
libxdo.xdo_move_window.__doc__ = """\
Move a window to a specific location.

The top left corner of the window will be moved to the x,y coordinate.

:param wid: the window to move
:param x: the X coordinate to move to.
:param y: the Y coordinate to move to.
"""

# ============================================================================
# int xdo_translate_window_with_sizehint(
#     const xdo_t *xdo, Window window,
#     unsigned int width, unsigned int height,
#     unsigned int *width_ret, unsigned int *height_ret);
libxdo.xdo_translate_window_with_sizehint.argtypes = (
    POINTER(xdo_t), window_t,
    c_uint, c_uint, POINTER(c_uint), POINTER(c_uint))
libxdo.xdo_translate_window_with_sizehint.restype = c_int
libxdo.xdo_translate_window_with_sizehint.errcheck = _errcheck
libxdo.xdo_translate_window_with_sizehint.__doc__ = """\
Apply a window's sizing hints (if any) to a given width and height.

This function wraps XGetWMNormalHints() and applies any
resize increment and base size to your given width and height values.

:param window: the window to use
:param width: the unit width you want to translate
:param height: the unit height you want to translate
:param width_ret: the return location of the translated width
:param height_ret: the return locatino of the translated height
"""


# ============================================================================
# int xdo_set_window_size(
#     const xdo_t *xdo, Window wid, int w, int h, int flags);
libxdo.xdo_set_window_size.argtypes = (
    POINTER(xdo_t), window_t, c_int, c_int, c_int)
libxdo.xdo_set_window_size.restype = c_int
libxdo.xdo_set_window_size.errcheck = _errcheck
libxdo.xdo_set_window_size.__doc__ = """\
Change the window size.

:param wid: the window to resize
:param w: the new desired width
:param h: the new desired height
:param flags: if 0, use pixels for units. If SIZE_USEHINTS, then
    the units will be relative to the window size hints.
"""


# ============================================================================
# int xdo_set_window_property(const xdo_t *xdo, Window wid,
#     const char *property, const char *value);
libxdo.xdo_set_window_property.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, c_char_p)
libxdo.xdo_set_window_property.restype = c_int
libxdo.xdo_set_window_property.errcheck = _errcheck
libxdo.xdo_set_window_property.__doc__ = """\
Change a window property.

Example properties you can change are WM_NAME, WM_ICON_NAME, etc.

:param wid: The window to change a property of.
:param property: the string name of the property.
:param value: the string value of the property.
"""


# ============================================================================
# int xdo_set_window_class(const xdo_t *xdo, Window wid, const char *name,
#                         const char *_class);
libxdo.xdo_set_window_class.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, c_char_p)
libxdo.xdo_set_window_class.restype = c_int
libxdo.xdo_set_window_class.errcheck = _errcheck
libxdo.xdo_set_window_class.__doc__ = """\
Change the window's classname and or class.

:param name: The new class name. If NULL, no change.
:param _class: The new class. If NULL, no change.
"""

# ============================================================================
# int xdo_set_window_urgency (const xdo_t *xdo, Window wid, int urgency);
libxdo.xdo_set_window_urgency.argtypes = (
    POINTER(xdo_t), window_t, c_int)
libxdo.xdo_set_window_urgency.restype = c_int
libxdo.xdo_set_window_urgency.errcheck = _errcheck
libxdo.xdo_set_window_urgency.__doc__ = """\
Sets the urgency hint for a window.
"""

# ============================================================================
# int xdo_set_window_override_redirect(const xdo_t *xdo, Window wid,
#                                      int override_redirect);
libxdo.xdo_set_window_override_redirect.argtypes = (
    POINTER(xdo_t), window_t, c_int)
libxdo.xdo_set_window_override_redirect.restype = c_int
libxdo.xdo_set_window_override_redirect.errcheck = _errcheck
libxdo.xdo_set_window_override_redirect.__doc__ = """\
Set the override_redirect value for a window. This generally means
whether or not a window manager will manage this window.

If you set it to 1, the window manager will usually not draw borders on the
window, etc. If you set it to 0, the window manager will see it like a
normal application window.
"""

# ============================================================================
# int xdo_focus_window(const xdo_t *xdo, Window wid);
libxdo.xdo_focus_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_focus_window.restype = c_int
libxdo.xdo_focus_window.errcheck = _errcheck
libxdo.xdo_focus_window.__doc__ = """\
Focus a window.

:see: xdo_activate_window
:param wid: the window to focus.
"""

# ============================================================================
# int xdo_raise_window(const xdo_t *xdo, Window wid);
libxdo.xdo_raise_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_raise_window.restype = c_int
libxdo.xdo_raise_window.errcheck = _errcheck
libxdo.xdo_raise_window.__doc__ = """\
Raise a window to the top of the window stack. This is also sometimes
termed as bringing the window forward.

:param wid: The window to raise.
"""

# ============================================================================
# int xdo_get_focused_window(const xdo_t *xdo, Window *window_ret);
libxdo.xdo_get_focused_window.argtypes = (POINTER(xdo_t), POINTER(window_t))
libxdo.xdo_get_focused_window.restype = c_int
libxdo.xdo_get_focused_window.errcheck = _errcheck
libxdo.xdo_get_focused_window.__doc__ = """\
Get the window currently having focus.

:param window_ret:
    Pointer to a window where the currently-focused window
    will be stored.
"""

# ============================================================================
# int xdo_wait_for_window_focus(const xdo_t *xdo, Window window,
#     int want_focus);
libxdo.xdo_wait_for_window_focus.argtypes = (
    POINTER(xdo_t), window_t, c_int)
libxdo.xdo_wait_for_window_focus.restype = c_int
libxdo.xdo_wait_for_window_focus.errcheck = _errcheck
libxdo.xdo_wait_for_window_focus.__doc__ = """\
Wait for a window to have or lose focus.

:param window: The window to wait on
:param want_focus: If 1, wait for focus. If 0, wait for loss of focus.
"""

# ============================================================================
# int xdo_get_pid_window(const xdo_t *xdo, Window window);
libxdo.xdo_get_pid_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_get_pid_window.restype = c_int
libxdo.xdo_get_pid_window.__doc__ = """\
Get the PID owning a window. Not all applications support this.
It looks at the _NET_WM_PID property of the window.

:param window: the window to query.
:return: the process id or 0 if no pid found.
"""

# ============================================================================
# int xdo_get_focused_window_sane(const xdo_t *xdo, Window *window_ret);
libxdo.xdo_get_focused_window_sane.argtypes = (
    POINTER(xdo_t), POINTER(window_t))
libxdo.xdo_get_focused_window_sane.restype = c_int
libxdo.xdo_get_focused_window_sane.errcheck = _errcheck
libxdo.xdo_get_focused_window_sane.__doc__ = """\
Like xdo_get_focused_window, but return the first ancestor-or-self window *
having a property of WM_CLASS. This allows you to get the "real" or
top-level-ish window having focus rather than something you may not expect
to be the window having focused.

:param window_ret:
    Pointer to a window where the currently-focused window
    will be stored.
"""

# ============================================================================
# int xdo_activate_window(const xdo_t *xdo, Window wid);
libxdo.xdo_activate_window.argtypes = (
    POINTER(xdo_t), window_t)
libxdo.xdo_activate_window.restype = c_int
libxdo.xdo_activate_window.errcheck = _errcheck
libxdo.xdo_activate_window.__doc__ = """\
Activate a window. This is generally a better choice than xdo_focus_window
for a variety of reasons, but it requires window manager support:
  - If the window is on another desktop, that desktop is switched to.
  - It moves the window forward rather than simply focusing it

Requires your window manager to support this.
Uses _NET_ACTIVE_WINDOW from the EWMH spec.

:param wid: the window to activate
"""

# ============================================================================
# int xdo_wait_for_window_active(const xdo_t *xdo, Window window, int active);
libxdo.xdo_wait_for_window_active.argtypes = (POINTER(xdo_t), window_t, c_int)
libxdo.xdo_wait_for_window_active.restype = c_int
libxdo.xdo_wait_for_window_active.errcheck = _errcheck
libxdo.xdo_wait_for_window_active.__doc__ = """\
Wait for a window to be active or not active.

Requires your window manager to support this.
Uses _NET_ACTIVE_WINDOW from the EWMH spec.

:param window: the window to wait on
:param active: If 1, wait for active. If 0, wait for inactive.
"""

# ============================================================================
# int xdo_map_window(const xdo_t *xdo, Window wid);
libxdo.xdo_map_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_map_window.restype = c_int
libxdo.xdo_map_window.errcheck = _errcheck
libxdo.xdo_map_window.__doc__ = """\
Map a window. This mostly means to make the window visible if it is
not currently mapped.

:param wid: the window to map.
"""

# ============================================================================
# int xdo_unmap_window(const xdo_t *xdo, Window wid);
libxdo.xdo_unmap_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_unmap_window.restype = c_int
libxdo.xdo_unmap_window.errcheck = _errcheck
libxdo.xdo_unmap_window.__doc__ = """\
Unmap a window

:param wid: the window to unmap
"""

# ============================================================================
# int xdo_minimize_window(const xdo_t *xdo, Window wid);
libxdo.xdo_minimize_window.argtypes = (POINTER(xdo_t), window_t)
libxdo.xdo_minimize_window.restype = c_int
libxdo.xdo_minimize_window.errcheck = _errcheck
libxdo.xdo_minimize_window.__doc__ = """\
Minimize a window.
"""

# ============================================================================
# int xdo_reparent_window(const xdo_t *xdo, Window wid_source,
#     Window wid_target);
libxdo.xdo_reparent_window.argtypes = (POINTER(xdo_t), window_t, window_t)
libxdo.xdo_reparent_window.restype = c_int
libxdo.xdo_reparent_window.errcheck = _errcheck
libxdo.xdo_reparent_window.__doc__ = """\
Reparents a window

:param wid_source: the window to reparent
:param wid_target: the new parent window
"""

# ============================================================================
# int xdo_get_window_location(const xdo_t *xdo, Window wid,
#                             int *x_ret, int *y_ret, Screen **screen_ret);
libxdo.xdo_get_window_location.argtypes = (
    POINTER(xdo_t), window_t, POINTER(c_int), POINTER(c_int),
    POINTER(Screen))
libxdo.xdo_get_window_location.restype = c_int
libxdo.xdo_get_window_location.errcheck = _errcheck
libxdo.xdo_get_window_location.__doc__ = """\
Get a window's location.

:param wid:
    the window to query
:param x_ret:
    pointer to int where the X location is stored. If NULL, X is
    ignored.
:param y_ret:
    pointer to int where the Y location is stored. If NULL, X is
    ignored.
:param screen_ret:
    Pointer to Screen* where the Screen* the window on is
    stored. If NULL, this parameter is ignored.
"""

# ============================================================================
# int xdo_get_window_size(const xdo_t *xdo, Window wid,
#     unsigned int *width_ret, unsigned int *height_ret);
libxdo.xdo_get_window_size.argtypes = (
    POINTER(xdo_t), window_t, POINTER(c_uint), POINTER(c_uint))
libxdo.xdo_get_window_size.restype = c_int
libxdo.xdo_get_window_size.errcheck = _errcheck
libxdo.xdo_get_window_size.__doc__ = """\
Get a window's size.

:param wid: the window to query
:param width_ret: pointer to unsigned int where the width is stored.
:param height_ret: pointer to unsigned int where the height is stored.
"""

# ============================================================================
# int xdo_get_active_window(const xdo_t *xdo, Window *window_ret);
libxdo.xdo_get_active_window.argtypes = (POINTER(xdo_t), POINTER(window_t))
libxdo.xdo_get_active_window.restype = c_int
libxdo.xdo_get_active_window.errcheck = _errcheck
libxdo.xdo_get_active_window.__doc__ = """\
Get the currently-active window.
Requires your window manager to support this.
Uses _NET_ACTIVE_WINDOW from the EWMH spec.

:param window_ret: Pointer to Window where the active window is stored.
"""

# ============================================================================
# int xdo_select_window_with_click(const xdo_t *xdo, Window *window_ret);
libxdo.xdo_select_window_with_click.argtypes = (
    POINTER(xdo_t), POINTER(window_t))
libxdo.xdo_select_window_with_click.restype = c_int
libxdo.xdo_select_window_with_click.errcheck = _errcheck
libxdo.xdo_select_window_with_click.__doc__ = """\
Get a window ID by clicking on it. This function blocks until a selection
is made.

:param window_ret: Pointer to Window where the selected window is stored.
"""

# ============================================================================
# int xdo_set_number_of_desktops(const xdo_t *xdo, long ndesktops);
libxdo.xdo_set_number_of_desktops.argtypes = (POINTER(xdo_t), c_long)
libxdo.xdo_set_number_of_desktops.restype = c_int
libxdo.xdo_set_number_of_desktops.errcheck = _errcheck
libxdo.xdo_set_number_of_desktops.__doc__ = """\
Set the number of desktops.
Uses _NET_NUMBER_OF_DESKTOPS of the EWMH spec.

:param ndesktops: the new number of desktops to set.
"""

# ============================================================================
# int xdo_get_number_of_desktops(const xdo_t *xdo, long *ndesktops);
libxdo.xdo_get_number_of_desktops.argtypes = (POINTER(xdo_t), POINTER(c_long))
libxdo.xdo_get_number_of_desktops.restype = c_int
libxdo.xdo_get_number_of_desktops.errcheck = _errcheck
libxdo.xdo_get_number_of_desktops.__doc__ = """\
Get the current number of desktops.
Uses _NET_NUMBER_OF_DESKTOPS of the EWMH spec.

:param ndesktops:
    pointer to long where the current number of desktops is stored
"""

# ============================================================================
# int xdo_set_current_desktop(const xdo_t *xdo, long desktop);
libxdo.xdo_set_current_desktop.argtypes = (POINTER(xdo_t), c_long)
libxdo.xdo_set_current_desktop.restype = c_int
libxdo.xdo_set_current_desktop.errcheck = _errcheck
libxdo.xdo_set_current_desktop.__doc__ = """\
Switch to another desktop.
Uses _NET_CURRENT_DESKTOP of the EWMH spec.

:param desktop: The desktop number to switch to.
"""

# ============================================================================
# int xdo_get_current_desktop(const xdo_t *xdo, long *desktop);
libxdo.xdo_get_current_desktop.argtypes = (POINTER(xdo_t), POINTER(c_long))
libxdo.xdo_get_current_desktop.restype = c_int
libxdo.xdo_get_current_desktop.errcheck = _errcheck
libxdo.xdo_get_current_desktop.__doc__ = """\
Get the current desktop.
Uses _NET_CURRENT_DESKTOP of the EWMH spec.

:param desktop: pointer to long where the current desktop number is stored.
"""

# ============================================================================
# int xdo_set_desktop_for_window(const xdo_t *xdo, Window wid, long desktop);
libxdo.xdo_set_desktop_for_window.argtypes = (POINTER(xdo_t), window_t, c_long)
libxdo.xdo_set_desktop_for_window.restype = c_int
libxdo.xdo_set_desktop_for_window.errcheck = _errcheck
libxdo.xdo_set_desktop_for_window.__doc__ = """\
Move a window to another desktop
Uses _NET_WM_DESKTOP of the EWMH spec.

:param wid: the window to move
:param desktop: the desktop destination for the window
"""

# ============================================================================
# int xdo_get_desktop_for_window(const xdo_t *xdo, Window wid, long *desktop);
libxdo.xdo_get_desktop_for_window.argtypes = (
    POINTER(xdo_t), window_t, POINTER(c_long))
libxdo.xdo_get_desktop_for_window.restype = c_int
libxdo.xdo_get_desktop_for_window.errcheck = _errcheck
libxdo.xdo_get_desktop_for_window.__doc__ = """\
Get the desktop a window is on.
Uses _NET_WM_DESKTOP of the EWMH spec.

If your desktop does not support _NET_WM_DESKTOP, then '*desktop' remains
unmodified.

:param wid: the window to query
:param desktop: pointer to long where the desktop of the window is stored
"""

# ============================================================================
# int xdo_search_windows(const xdo_t *xdo, const xdo_search_t *search,
#                       Window **windowlist_ret, unsigned int *nwindows_ret);
libxdo.xdo_search_windows.argtypes = (
    POINTER(xdo_t), POINTER(xdo_search_t), POINTER(POINTER(window_t)),
    POINTER(c_uint))
libxdo.xdo_search_windows.restype = c_int
libxdo.xdo_search_windows.errcheck = _errcheck
libxdo.xdo_search_windows.__doc__ = """\
Search for windows.

:param search: the search query.
:param windowlist_ret: the list of matching windows to return
:param nwindows_ret: the number of windows (length of windowlist_ret)
:see: xdo_search_t
"""

# ============================================================================
# unsigned char *xdo_get_window_property_by_atom(
#     const xdo_t *xdo, Window window, Atom atom,
#     long *nitems, Atom *type, int *size);
# todo: fix the ``atom_t`` thing
# todo: how to error check this function? NULL pointer return?
libxdo.xdo_get_window_property_by_atom.argtypes = (
    POINTER(xdo_t), window_t, atom_t, POINTER(c_long),
    POINTER(atom_t), POINTER(c_int))
libxdo.xdo_get_window_property_by_atom.restype = c_char_p
libxdo.xdo_get_window_property_by_atom.__doc__ = """
Generic property fetch.

:param window: the window to query
:param atom: the Atom to request
:param nitems: the number of items
:param type: the type of the return
:param size: the size of the type
:return:
    data consisting of 'nitems' items of size 'size' and type 'type'
    will need to be cast to the type before using.
"""

# ============================================================================
# int xdo_get_window_property(
#     const xdo_t *xdo, Window window, const char *property,
#     unsigned char **value, long *nitems, Atom *type, int *size);
libxdo.xdo_get_window_property.argtypes = (
    POINTER(xdo_t), window_t, c_char_p, POINTER(c_char_p), POINTER(c_long),
    POINTER(atom_t), POINTER(c_int))
libxdo.xdo_get_window_property.restype = c_int
libxdo.xdo_get_window_property.errcheck = _errcheck
libxdo.xdo_get_window_property.__doc__ = """\
Get property of window by name of atom.

:param window: the window to query
:param property: the name of the atom
:param nitems: the number of items
:param type: the type of the return
:param size: the size of the type
:return: data consisting of 'nitems' items of size 'size' and type 'type'
    will need to be cast to the type before using.
"""


# ============================================================================
# unsigned int xdo_get_input_state(const xdo_t *xdo);
# todo: we need to define constants for the mask!
libxdo.xdo_get_input_state.argtypes = (POINTER(xdo_t),)
libxdo.xdo_get_input_state.restype = c_uint
libxdo.xdo_get_input_state.__doc__ = """
Get the current input state. This is a mask value containing any of the
following: ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask,
Mod4Mask, or Mod5Mask.

:return: the input mask
"""

# ============================================================================
# const char **xdo_get_symbol_map(void);
libxdo.xdo_get_symbol_map.argtypes = ()
libxdo.xdo_get_symbol_map.restype = POINTER(c_char_p)
libxdo.xdo_get_symbol_map.__doc__ = """
If you need the symbol map, use this method.

The symbol map is an array of string pairs mapping common tokens to X Keysym
strings, such as "alt" to "Alt_L"

:return: array of strings.
"""

# ============================================================================
# int xdo_get_active_modifiers(const xdo_t *xdo, charcodemap_t **keys,
#                              int *nkeys);
libxdo.xdo_get_active_modifiers.argtypes = (
    POINTER(xdo_t), POINTER(POINTER(charcodemap_t)), POINTER(c_int))
libxdo.xdo_get_active_modifiers.restype = c_int
libxdo.xdo_get_active_modifiers.errcheck = _errcheck
libxdo.xdo_get_active_modifiers.__doc__ = """\
Get a list of active keys. Uses XQueryKeymap.

:param keys: Pointer to the array of charcodemap_t that will be allocated
   by this function.
:param nkeys: Pointer to integer where the number of keys will be stored.
"""

# ============================================================================
# int xdo_clear_active_modifiers(const xdo_t *xdo, Window window,
#                                charcodemap_t *active_mods,
#                                int active_mods_n);
libxdo.xdo_clear_active_modifiers.argtypes = (
    POINTER(xdo_t), window_t, POINTER(charcodemap_t), c_int)
libxdo.xdo_clear_active_modifiers.restype = c_int
libxdo.xdo_clear_active_modifiers.errcheck = _errcheck
libxdo.xdo_clear_active_modifiers.__doc__ = """\
Send any events necesary to clear the the active modifiers.
For example, if you are holding 'alt' when xdo_get_active_modifiers is
called, then this method will send a key-up for 'alt'
"""

# ============================================================================
# int xdo_set_active_modifiers(const xdo_t *xdo, Window window,
#                              charcodemap_t *active_mods,
#                              int active_mods_n);
libxdo.xdo_set_active_modifiers.argtypes = (
    POINTER(xdo_t), window_t, POINTER(charcodemap_t), c_int)
libxdo.xdo_set_active_modifiers.restype = c_int
libxdo.xdo_set_active_modifiers.errcheck = _errcheck
libxdo.xdo_set_active_modifiers.__doc__ = """\
Send any events necessary to make these modifiers active.
This is useful if you just cleared the active modifiers and then wish
to restore them after.
"""

# ============================================================================
# int xdo_get_desktop_viewport(const xdo_t *xdo, int *x_ret, int *y_ret);
libxdo.xdo_get_desktop_viewport.argtypes = (
    POINTER(xdo_t), POINTER(c_int), POINTER(c_int))
libxdo.xdo_get_desktop_viewport.restype = c_int
libxdo.xdo_get_desktop_viewport.errcheck = _errcheck
libxdo.xdo_get_desktop_viewport.__doc__ = """\
Get the position of the current viewport.

This is only relevant if your window manager supports
``_NET_DESKTOP_VIEWPORT``
"""

# ============================================================================
# int xdo_set_desktop_viewport(const xdo_t *xdo, int x, int y);
libxdo.xdo_set_desktop_viewport.argtypes = (
    POINTER(xdo_t), c_int, c_int)
libxdo.xdo_set_desktop_viewport.restype = c_int
libxdo.xdo_set_desktop_viewport.errcheck = _errcheck
libxdo.xdo_set_desktop_viewport.__doc__ = """\
Set the position of the current viewport.

This is only relevant if your window manager supports
``_NET_DESKTOP_VIEWPORT``
"""

# ============================================================================
# int xdo_kill_window(const xdo_t *xdo, Window window);
libxdo.xdo_kill_window.argtypes = (
    POINTER(xdo_t), window_t)
libxdo.xdo_kill_window.restype = c_int
libxdo.xdo_kill_window.errcheck = _errcheck
libxdo.xdo_kill_window.__doc__ = """\
Kill a window and the client owning it.
"""

# ============================================================================
# Constants for xdo_find_window_client()

# Find a client window that is a parent of the window given
XDO_FIND_PARENTS = 0

# Find a client window that is a child of the window given
XDO_FIND_CHILDREN = 1

# ============================================================================
# int xdo_find_window_client(const xdo_t *xdo, Window window,
#     Window *window_ret, int direction);
libxdo.xdo_find_window_client.argtypes = (
    POINTER(xdo_t), window_t, POINTER(window_t), c_int)
libxdo.xdo_find_window_client.restype = c_int
libxdo.xdo_find_window_client.errcheck = _errcheck
libxdo.xdo_find_window_client.__doc__ = """\
Find a client window (child) in a given window. Useful if you get the
window manager's decorator window rather than the client window.
"""

# ============================================================================
# int xdo_get_window_name(const xdo_t *xdo, Window window,
#                         unsigned char **name_ret, int *name_len_ret,
#                         int *name_type);
libxdo.xdo_get_window_name.argtypes = (
    POINTER(xdo_t), window_t, POINTER(c_char_p), POINTER(c_int),
    POINTER(c_int))
libxdo.xdo_get_window_name.restype = c_int
libxdo.xdo_get_window_name.errcheck = _errcheck
libxdo.xdo_get_window_name.__doc__ = """\
Get a window's name, if any.

TODO(sissel): Document
"""

# ============================================================================
# void xdo_disable_feature(xdo_t *xdo, int feature);
libxdo.xdo_disable_feature.argtypes = (POINTER(xdo_t), c_int)
libxdo.xdo_disable_feature.restype = None
libxdo.xdo_disable_feature.__doc__ = """\
Disable an xdo feature.

This function is mainly used by libxdo itself, however, you may find it useful
in your own applications.

:see: XDO_FEATURES
"""

# ============================================================================
# void xdo_enable_feature(xdo_t *xdo, int feature);
libxdo.xdo_enable_feature.argtypes = (POINTER(xdo_t), c_int)
libxdo.xdo_enable_feature.restype = None
libxdo.xdo_enable_feature.__doc__ = """\
Enable an xdo feature.

This function is mainly used by libxdo itself, however, you may find it useful
in your own applications.

:see: XDO_FEATURES
"""

# ============================================================================
# int xdo_has_feature(xdo_t *xdo, int feature);
libxdo.xdo_has_feature.argtypes = (POINTER(xdo_t), c_int)
libxdo.xdo_has_feature.restype = c_int
libxdo.xdo_has_feature.errcheck = _errcheck
libxdo.xdo_has_feature.__doc__ = """\
Check if a feature is enabled.

This function is mainly used by libxdo itself, however, you may find it useful
in your own applications.

:see: XDO_FEATURES
"""

# ============================================================================
# int xdo_get_viewport_dimensions(xdo_t *xdo, unsigned int *width,
#                                 unsigned int *height, int screen);
libxdo.xdo_get_viewport_dimensions.argtypes = (
    POINTER(xdo_t), POINTER(c_uint), POINTER(c_uint), c_int)
libxdo.xdo_get_viewport_dimensions.restype = c_int
libxdo.xdo_get_viewport_dimensions.errcheck = _errcheck
libxdo.xdo_get_viewport_dimensions.__doc__ = """\
Query the viewport (your display) dimensions

If Xinerama is active and supported, that api internally is used.
If Xineram is disabled, we will report the root window's dimensions
for the given screen.
"""

# ============================================================================
# void XFree(void *data);
libX11.XFree.argtypes = (c_void_p,)
libX11.XFree.restype = None
libX11.XFree.__doc__ = """\
The XFree function is a general-purpose Xlib routine that frees the
specified data.

You must use it to free any objects that were allocated by Xlib,
unless an alternate function is explicitly specified for the object. A
NULL pointer cannot be passed to this function.

:param data: Specifies the pointer to data that is to be freed
"""

# ============================================================================
# typedef int (*XErrorHandler) (      /* WARNING, this type not in Xlib spec */
#     Display*            /* display */,
#     XErrorEvent*        /* error_event */
# );
XErrorHandler = ctypes.CFUNCTYPE(c_int, POINTER(XErrorEvent))
# ============================================================================
