# macOS

Open `app/Eigenmath.xcodeproj` in Xcode,
press ⌘R to build and run.

[Eigenmath Manual for macOS](https://georgeweigt.github.io/eigenmath-macos.pdf)

# Linux

In the current directory, do

```
make
```

Then

```
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

[Eigenmath Manual for Linux](https://georgeweigt.github.io/eigenmath.pdf)

# Javascript

eigenmath.js is a Javascript file that can evaluate an Eigenmath script.

The script is read from `document.getElementById("stdin").value`

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

<button onclick="run()" style="font-size:20pt">Run</button>

<p>
<div id="stdout"></div>

</body>
</html>
```

To build eigenmath.js

```
cd js
make
```
