[Eigenmath Home Page](https://georgeweigt.github.io)
#

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

To run test scripts

```
./eigenmath test/selftest1
./eigenmath test/selftest2
```

arch/manjaro install from aur

```
paru -S eigenmath-git
```

#

`eigenmath.js` is a Javascript program for evaluating Eigenmath scripts in a browser.

Scripts are read from `document.getElementById("stdin").value`

Results are written to `document.getElementById("stdout").innerHTML`

For example (click [here](https://georgeweigt.github.io/demo.html) to try)

```
<html>
<body>

<script src="https://georgeweigt.github.io/eigenmath.js"></script>

<textarea id="stdin" rows="24" cols="80" style="font-family:courier;font-size:1em">
-- Eigenmath script goes here
f = sin(x) / x
f
yrange = (-1,1)
draw(f,x)
</textarea>

<button onclick="run()">Run</button><br>

<div id="stdout"></div>

</body>
</html>
```
