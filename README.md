
To build and run

```
make
./eigenmath
```

Press ctrl-C to exit.

Eigenmath uses UTF-8 encoding. The terminal window should have UTF-8 enabled.

To run a script

```
./eigenmath scriptfilename
```

Sample scripts are available at [georgeweigt.github.io](https://georgeweigt.github.io)

To run test scripts

```
./eigenmath test/selftest1
./eigenmath test/selftest2
```

If something goes wrong, "Stop" is printed.
Otherwise, all is well.

#
#

To build and install for macOS

1. Install Xcode from the Mac App Store.

2. Launch Xcode and open the project `eigenmath-master/xcode/Eigenmath.xcodeproj`

3. From the Xcode Product menu, select Archive.
Xcode starts building Eigenmath, which takes some time.

4. Eventually the Organizer window opens.
Click Distribute Content.

5. Choose Built Products (the default) then click Next.

6. Click Export to save the build somewhere such as Desktop.

7. In macOS Finder, go to the folder that Organizer exported.

8. Click through nested folders until the Eigenmath app is found.

9. Drag Eigenmath to the macOS Applications folder.

#
#

eigenmath.js is a Javascript file that can evaluate an Eigenmath script.

Scripts are read from `document.getElementById("stdin").value`

Results are written to `document.getElementById("stdout").innerHTML`

For example (click [here](https://georgeweigt.github.io/demo.html) to try)

```html
<html>
<body>

<script src="https://georgeweigt.github.io/eigenmath.js"></script>

<textarea id="stdin" rows="24" cols="80" style="font-family:courier;font-size:12pt">
-- Eigenmath script goes here
f = sin(x) / x
f
yrange = (-1,1)
draw(f,x)
</textarea>

<button onclick="run()">Run</button>

<p>
<div id="stdout"></div>

</body>
</html>
```
