# YACM
Yet Another Console Manager

# Setup
Before building the app, you should make sure that you've already installed ncurses. To build the app, just copy the repository and go as usual:
 * mkdir build
 * cd build
 * cmake ..
 * make YACM
 * ./YACM
# Guidance
 * Use up and down arrows to navigate.
 * Use ```w``` or ```W``` to go into a directory or try open a file. 
 * Use ```h``` or ```H``` to switch between hidden files displaying modes. 
 * Use ```c``` or ```C``` to copy file, the file will be highlighted until pasted.
 * Use ```v``` or ```V``` to past file respectively, and ```x``` to past with cut.
 * Use ```q``` or ```Q``` to quit.

At the bottom there is a numbering of files and a log with possible errors. For example, if you try to open a file or directory without the appropriate permissions, a message informing you about this will be displayed.
