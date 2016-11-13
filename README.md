# z2splits

A splits tracker for Zelda 2.

![screenshot](https://raw.github.com/bentglasstube/z2splits/master/screenshot.png)

Shows your cumulative time and delta from best (if you have a best) and
indicates "golds" with a triforce icon.  Automatically saves your best times
for each split after a completed run.

## Controls

When stopped:

|Key        |Action|
|-----------|------|
|Space      |Start timer|
|Escape     |Reset tracker|

When running:

|Key        |Action|
|-----------|------|
|Space      |Next split|
|Backspace  |Previous split|
|Tab        |Skip split|
|Return     |Stop timer|
|Escape     |Reset tracker and stop|
|Up/Down    |Scroll split list|

## Modifying Splits

You can edit the `content/z2allkeys.txt` file to modify the names and orders of
the splits to your liking.  The first line of the flie has the start delay (so
you can start the timer when you press start to pick a file) and the title to
show.  Every following line shows a split.  The split format is a hint number,
followed by the best time for that split (in ms), and then the name of the
split.  If you mess up the format of the file, the tracker will probably crash
or behave in other undesirable ways.
