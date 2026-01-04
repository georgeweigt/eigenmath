This app provides a unified environment for symbolic manipulation, numerical computation, and visualization of mathematical objects.
This app supports a wide range of mathematical operations, including symbolic algebra, multivariable calculus, and matrix operations.

Commands are entered at the `?` prompt.
Press ctrl-C to exit.

```
% make
% ./eigenmath
? 212^17
3529471145760275132301897342055866171392

? d(erf(x),x)
      ┌   ┐ 
      │  2│ 
 2 exp│−x │ 
      │   │ 
      └   ┘ 
╶──────────╴
     1/2    
    π       

? ^C
%
```

Scripts are run from the command line.

```
% ./eigenmath demo/spontaneous-emission-rate
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

       3 α c 
ω   = ╶─────╴
 21    8 a   
          0  

            4   
       256 α  c 
A   = ╶────────╴
 21    6561 a   
             0  

Spontaneous emission rate

                8 
       6.2649×10  
A   = ╶──────────╴
 21      second   

Verify emission rate

ok

? ^C
%
```
