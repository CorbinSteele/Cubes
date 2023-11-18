# Cubes
3D rendering of a cube calendar using JUCE

## What is it?
This is a desktop app that allows setting the current date on a cube calendar. It renders using JUCE and OpenGL, is capable of realtime syncing of the rotation and handling of the cubes among multiple users, and minimizes to the taskbar in an unobtrusive way.

## How do I run it?
If you're familiar with [JUCE](https://github.com/juce-framework/JUCE), then building it yourself should be very straightforward using the included .jucer file with either JUCE v6 or v7. Otherwise, if you're the lazy yet trustworthy sort, and you use Windows, then you should be able to simply use the released exe file. (There's also an executable for Linux, if you happen to use a similar version of Ubuntu Linux as me, but your mileage may vary. Different Linux taskbars sometimes behave differently.)

## How do I use the realtime syncing?
You don't. It's designed to work with my personal server, whose code is not included. It would be very simple to get it working yourself though: the server really just relays data without doing any processing itself.

## What does it look like?
while playing with the current date | while minimized
-- | --
![a cube calendar](Images/cube-calendar.png) | ![a small cube calendar](Images/taskbar-cube-calendar.png)
