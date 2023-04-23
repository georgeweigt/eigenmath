[Eigenmath Home Page](https://georgeweigt.github.io)

#

Eigenmath uses UTF-8 encoding to display Greek letters and other symbols.

To build and run

```
make
./eigenmath
```

Press ctrl-C to exit.

To run a script

```
./eigenmath scriptfilename
```

To run test scripts

```
./eigenmath test/selftest1
./eigenmath test/selftest2
```

Run spontaneous emission demo

```
./eigenmath spontaneous-emission 
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

#

Packages

[ports.macports.org](https://ports.macports.org/port/eigenmath/)
```
sudo port install eigenmath
```

[aur.archlinux.org](https://aur.archlinux.org/packages/eigenmath-git)
```
paru -S eigenmath-git
```
