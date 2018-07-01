gdbplz
======

C++11 library frontend for GNU Debugger.

(disclaimer: this library is not even in the alpha phase)

Features
--------

- forward-compatible (in the sense that newer releases of GDB won't break the frontend)
- permissive license (MIT)
- not letting run ambiguous commands

Dependencies
------------

For library:

- Boost.Filesystem
- Boost.System
- tiny-process-library

For test suite you need all of the above, plus:

- Catch
- Rapidcheck

Potentially Asked Questions
---------------------------

Q: GDB is licensed with GNU GPL and your library is MIT-licensed. Isn't that violation of GPL license?

A: [No, it's not](https://www.gnu.org/copyleft/gpl-faq.html#GPLInProprietarySystem). The GDB is not linked, it's run as a separate process, which meets the requirements mentioned here.

