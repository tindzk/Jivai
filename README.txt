Jivai
-----

Jivai aims to be a  modern libc using recent programming techniques.
Hence, it does not implement the POSIX specification as it follows a
completely different approach.

Although Jivai is written in C, some design decisions had to be made
in  order  to  achieve  a  certain  object-orientation  as  well as
consistency.

Another strength of Jivai is in its performance: Many modules can be
fine-tuned  to gain  speed improvements  or  to  act  differently in
production environments.  The fact that Jivai is Linux-only makes it
possible to  prevent slow workarounds  and unnecessary compatibility
code.  Furthermore,  some  modules were  optimized for  Linux unique
capabilities, e.g. leading to a reduction of system calls.

Jivai  facilitates  debugging:   There   is  an  advanced  exception
infrastructure which is being used throughout most modules. Critical
errors don't get  ignored any more in case you  forgot to check some
return  values.  Moreover,  it is  possible to  obtain a  call trace
whenever an exception occurs.

There are  straightforward implementations  of data  structures like
linked lists, strings and arrays. Adding elements will automatically
allocate more  memory if necessary.  You  no longer need  to scatter
routines for memory allocation over your code.  However, Jivai still
offers the  choice to control the  memory allocation yourself.  This
might come  in handy if  the implemented techniques  are too generic
for your purposes.

As Jivai uses a special C variant,  Clang is the preferred compiler.
In the course of producing object-oriented code, the code also makes
use of less common features such as closures which are not supported
natively by any other compiler.

Unfortunately,  currently Jivai  still  requires  a POSIX-compatible
libc. Luckily, it only requires a minimal subset, making it possible
to use lightweight libc's like the diet libc.

Jivai has been successfully used for the following areas so far:
  * Network programming
  * Terminal applications
  * Event-driven programming

Features include:
  * Performance
  * Consistency
  * Exception support
  * Object-oriented code
  * Debugging facilities
  * Fine-tunable
  * Low footprint
  * Linux-only
  * GPLv2-licensed
