[Eigenmath Home Page](https://georgeweigt.github.io)

#

To build and run for Linux

```
make
./eigenmath
```

Press ctrl-C to exit.

To run a script (example)

```
./eigenmath demo/spontaneous-emission
```

Result

```
Spontaneous emission rate for hydrogen state 2p

                   ┌       ┐ 
                   │       │ 
                   │  3 r  │ 
       r cos(θ) exp│−╶────╴│ 
                   │  2 a  │ 
                   │     0 │ 
                   └       ┘ 
f   = ╶─────────────────────╴
 21            1/2  4        
            4 2    a  π      
                    0        

x   = 0
 21    

y   = 0
 21    

       128   1/2   
z   = ╶───╴ 2    a 
 21    243        0

         32768   2
r     = ╶─────╴ a 
 21sq    59049   0

Spontaneous emission rate

                8 
       6.2649×10  
A   = ╶──────────╴
 21      second   

Mean interval

          −9       
1.59619×10   second
```

Eigenmath uses UTF-8 encoding to display Greek letters and other symbols.

To run self tests

```
./eigenmath test/selftest1
./eigenmath test/selftest2
```

#

Go to the `xcode` directory to build for macOS.

See also directory `js` for a Javascript version.
