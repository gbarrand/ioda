*
*
>Name gopaw_kuip_def

****************************************************************
****************************************************************
****************************************************************
>Menu /SESSION
>Guidance
To work on the session.

>Command NOW
>Guidance
Set session inlib::atime to "now".
>Action gopaw_kuip%C

>Command ELAPSED
>Guidance
Print elapsed time since last NOW.
>Action gopaw_kuip%C

>Command MOVE
>Parameters
FROM 'Old file name' C
TO   'New file name' C
>Keyword
Rename move
>Guidance
Rename a local file.
>Action gopaw_kuip%C

>Command LLS
>Keyword
List current directory
>Guidance
List current directory. (Use LCD to know or change current directory).
>Action gopaw_kuip%C

>Command PYTHON
>Parameters
FILE '.py file name' C
>Keyword
Python py
>Guidance
Execute Python .py file.
>Action gopaw_kuip%C

>Command DELETE
>Parameters
ID 'Object Identifier' C Loop
>Keyword
delete
>Guidance
Delete objects handled by the gopaw::session with name ID. ID can contain '*' widcards.
>Action gopaw_kuip%C

****************************************************************
****************************************************************
****************************************************************

>Menu /VIEWER
>Guidance
To work on a viewer widget.

>Command RENDER
>Parameters
*NAME 'Name of the viewer' C D='@current@'
>Guidance
Force a redraw.
>Action gopaw_kuip%C

>Command SYNCHRONIZE
>Guidance
Force a graphics synchronization with the server (if any).
>Action gopaw_kuip%C

>Command BACKGROUND_COLOR
>Parameters
R 'Red in [0,1]'   R D=1
G 'Green in [0,1]' R D=1
B 'Blue in [0,1]'  R D=1
A 'Alpha in [0,1]' R D=1
>Guidance
Set window background color.
>Action gopaw_kuip%C

>Menu PAGE
>Guidance
To work on a page (= inlib::sg::plots).

>Command SET_LAYOUT
>Guidance
Set layout of the page.
>Action gopaw_kuip%C

*>Menu ../PLOTTER
*>Guidance
*To work on an inlib::sg::plotter.

*>Command SET_STYLE
*>Parameters
*STYLE 'style name' C
*>Keyword
*Set the overall style of the current inlib::sg::plotter.
*STYLE must be the name of a known style.
*For example ROOT_default, hippodraw, inlib_default.
*>Guidance
*Rename a local file.
*>Action gopaw_kuip%C


