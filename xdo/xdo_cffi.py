from cffi import FFI
ffi = FFI()

ffi.set_source('_xdo', """
#include <xdo.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xmd.h>
""", libraries=['xdo', 'X11'])

ffi.cdef("""
#define SIZE_USEHINTS ...
#define SIZE_USEHINTS_X ...
#define SIZE_USEHINTS_Y ...
#define CURRENTWINDOW ...

typedef ... KeyCode;
typedef ... KeySym;
typedef ... Display;
/* typedef ... CARD32; */
/* typedef CARD32 XID; */
typedef ... XID;
typedef ... Window;
typedef ... useconds_t;
typedef ... Screen;
typedef ... Atom;

typedef struct charcodemap {
  wchar_t key;
  KeyCode code;
  KeySym symbol;
  int group;
  int modmask;
  int needs_binding;
} charcodemap_t;

typedef enum {
  XDO_FEATURE_XTEST,
} XDO_FEATURES;


typedef struct xdo {
  Display *xdpy;
  char *display_name;
  charcodemap_t *charcodes;
  int charcodes_len;
  int keycode_high;
  int keycode_low;
  int keysyms_per_keycode;
  int close_display_when_freed;
  int quiet;
  int debug;
  int features_mask;
} xdo_t;


#define SEARCH_TITLE ...
#define SEARCH_CLASS ...
#define SEARCH_NAME ...
#define SEARCH_PID ...
#define SEARCH_ONLYVISIBLE ...
#define SEARCH_SCREEN ...
#define SEARCH_CLASSNAME ...
#define SEARCH_DESKTOP ...

typedef struct xdo_search {
  const char *title;
  const char *winclass;
  const char *winclassname;
  const char *winname;
  int pid;
  long max_depth;
  int only_visible;
  int screen;
  enum { SEARCH_ANY, SEARCH_ALL } require;
  unsigned int searchmask;
  long desktop;
  unsigned int limit;
} xdo_search_t;

#define XDO_ERROR ...
#define XDO_SUCCESS ...

xdo_t* xdo_new(const char *display);

xdo_t* xdo_new_with_opened_display(Display *xdpy, const char *display,
                                   int close_display_when_freed);

const char *xdo_version(void);
void xdo_free(xdo_t *xdo);
int xdo_move_mouse(const xdo_t *xdo, int x, int y, int screen);
int xdo_move_mouse_relative_to_window(const xdo_t *xdo, Window window, int x, int y);
int xdo_move_mouse_relative(const xdo_t *xdo, int x, int y);
int xdo_mouse_down(const xdo_t *xdo, Window window, int button);
int xdo_mouse_up(const xdo_t *xdo, Window window, int button);
int xdo_get_mouse_location(const xdo_t *xdo, int *x, int *y, int *screen_num);
int xdo_get_window_at_mouse(const xdo_t *xdo, Window *window_ret);
int xdo_get_mouse_location2(const xdo_t *xdo, int *x_ret, int *y_ret,
                            int *screen_num_ret, Window *window_ret);
int xdo_wait_for_mouse_move_from(const xdo_t *xdo, int origin_x, int origin_y);
int xdo_wait_for_mouse_move_to(const xdo_t *xdo, int dest_x, int dest_y);
int xdo_click_window(const xdo_t *xdo, Window window, int button);
int xdo_click_window_multiple(const xdo_t *xdo, Window window, int button,
                       int repeat, useconds_t delay);
int xdo_enter_text_window(const xdo_t *xdo, Window window, const char *string, useconds_t delay);
int xdo_send_keysequence_window(const xdo_t *xdo, Window window,
                    const char *keysequence, useconds_t delay);
int xdo_send_keysequence_window_up(const xdo_t *xdo, Window window,
                       const char *keysequence, useconds_t delay);
int xdo_send_keysequence_window_down(const xdo_t *xdo, Window window,
                         const char *keysequence, useconds_t delay);
int xdo_send_keysequence_window_list_do(const xdo_t *xdo, Window window,
                            charcodemap_t *keys, int nkeys,
                            int pressed, int *modifier, useconds_t delay);
/*int xdo_get_active_keys_to_keycode_list(const xdo_t *xdo, charcodemap_t **keys,
                                         int *nkeys);*/
int xdo_wait_for_window_map_state(const xdo_t *xdo, Window wid, int map_state);
#define SIZE_TO ...
#define SIZE_FROM ...
int xdo_wait_for_window_size(const xdo_t *xdo, Window window, unsigned int width,
                             unsigned int height, int flags, int to_or_from);
int xdo_move_window(const xdo_t *xdo, Window wid, int x, int y);
int xdo_translate_window_with_sizehint(const xdo_t *xdo, Window window,
                                       unsigned int width, unsigned int height,
                                       unsigned int *width_ret, unsigned int *height_ret);
int xdo_set_window_size(const xdo_t *xdo, Window wid, int w, int h, int flags);
int xdo_set_window_property(const xdo_t *xdo, Window wid, const char *property,
                        const char *value);
int xdo_set_window_class(const xdo_t *xdo, Window wid, const char *name,
                        const char *_class);
int xdo_set_window_urgency (const xdo_t *xdo, Window wid, int urgency);
int xdo_set_window_override_redirect(const xdo_t *xdo, Window wid,
                                     int override_redirect);
int xdo_focus_window(const xdo_t *xdo, Window wid);
int xdo_raise_window(const xdo_t *xdo, Window wid);
int xdo_get_focused_window(const xdo_t *xdo, Window *window_ret);
int xdo_wait_for_window_focus(const xdo_t *xdo, Window window, int want_focus);
int xdo_get_pid_window(const xdo_t *xdo, Window window);
int xdo_get_focused_window_sane(const xdo_t *xdo, Window *window_ret);
int xdo_activate_window(const xdo_t *xdo, Window wid);
int xdo_wait_for_window_active(const xdo_t *xdo, Window window, int active);
int xdo_map_window(const xdo_t *xdo, Window wid);
int xdo_unmap_window(const xdo_t *xdo, Window wid);
int xdo_minimize_window(const xdo_t *xdo, Window wid);
int xdo_reparent_window(const xdo_t *xdo, Window wid_source, Window wid_target);
int xdo_get_window_location(const xdo_t *xdo, Window wid,
                            int *x_ret, int *y_ret, Screen **screen_ret);
int xdo_get_window_size(const xdo_t *xdo, Window wid, unsigned int *width_ret,
                        unsigned int *height_ret);
int xdo_get_active_window(const xdo_t *xdo, Window *window_ret);
int xdo_select_window_with_click(const xdo_t *xdo, Window *window_ret);
int xdo_set_number_of_desktops(const xdo_t *xdo, long ndesktops);
int xdo_get_number_of_desktops(const xdo_t *xdo, long *ndesktops);
int xdo_set_current_desktop(const xdo_t *xdo, long desktop);
int xdo_get_current_desktop(const xdo_t *xdo, long *desktop);
int xdo_set_desktop_for_window(const xdo_t *xdo, Window wid, long desktop);
int xdo_get_desktop_for_window(const xdo_t *xdo, Window wid, long *desktop);
int xdo_search_windows(const xdo_t *xdo, const xdo_search_t *search,
                      Window **windowlist_ret, unsigned int *nwindows_ret);
unsigned char *xdo_get_window_property_by_atom(const xdo_t *xdo, Window window, Atom atom,
                              long *nitems, Atom *type, int *size);
int xdo_get_window_property(const xdo_t *xdo, Window window, const char *property,
                            unsigned char **value, long *nitems, Atom *type, int *size);
unsigned int xdo_get_input_state(const xdo_t *xdo);
const char **xdo_get_symbol_map(void);
int xdo_get_active_modifiers(const xdo_t *xdo, charcodemap_t **keys,
                             int *nkeys);
int xdo_clear_active_modifiers(const xdo_t *xdo, Window window,
                               charcodemap_t *active_mods,
                               int active_mods_n);
int xdo_set_active_modifiers(const xdo_t *xdo, Window window,
                             charcodemap_t *active_mods,
                             int active_mods_n);
int xdo_get_desktop_viewport(const xdo_t *xdo, int *x_ret, int *y_ret);
int xdo_set_desktop_viewport(const xdo_t *xdo, int x, int y);
int xdo_kill_window(const xdo_t *xdo, Window window);
#define XDO_FIND_PARENTS ...
#define XDO_FIND_CHILDREN ...
int xdo_find_window_client(const xdo_t *xdo, Window window, Window *window_ret,
                           int direction);
int xdo_get_window_name(const xdo_t *xdo, Window window,
                        unsigned char **name_ret, int *name_len_ret,
                        int *name_type);
void xdo_disable_feature(xdo_t *xdo, int feature);
void xdo_enable_feature(xdo_t *xdo, int feature);
int xdo_has_feature(xdo_t *xdo, int feature);
int xdo_get_viewport_dimensions(xdo_t *xdo, unsigned int *width,
                                unsigned int *height, int screen);
""")

ffi.dlopen(None)

if __name__ == '__main__':
    ffi.compile()
