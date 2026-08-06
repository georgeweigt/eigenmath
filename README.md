This app provides a unified environment for symbolic manipulation, numerical computation, and visualization of mathematical objects.
This app supports a wide range of mathematical operations, including symbolic algebra, multivariable calculus, and matrix operations.

Build and run

```
make
./eigenmath
```

Commands are entered at the `?` prompt.

```
? 212^17
3529471145760275132301897342055866171392

? polar(1 + i)
        ┌       ┐
 1/2    │ 1     │
2    exp│╶─╴ i π│
        │ 4     │
        └       ┘

? d(sin(x),x)
cos(x)

? exit
```

Scripts are run from the command line.

```
./eigenmath demo/spontaneous-emission-rate
```
