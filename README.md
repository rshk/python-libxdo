# Python bindings for libxdo

libxdo is the library behind [xdotool](https://github.com/jordansissel/xdotool),
a tool to "fake keyboard/mouse input, window management, and more".

This package aims to provide ctypes-based Python bindings (and a more pythonic
way to use the C library, of course..)


**WARNING** The project is still work in progress!
Only a small part of the library is supported, and tests / documentation are missing.
Use with care (and make sure to report any issue you should find!)


## Documentation

Documentation is hosted on Read The Docs:

http://python-libxdo.readthedocs.org/


## Example usage

```python
from xdo import Xdo
xdo = Xdo()
win_id = xdo.select_window_with_click()
xdo.enter_text_window(win_id, 'Python rocks!')
```
