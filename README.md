# Simple SHA-1 implementation in C

Hopefully, will help for anyone, who will investigate SHA-1.

Please, do not use SHA-1 in real programs: [it is not secure](https://shattered.io/).

## Build and run

Build:

```
make
```

Or if no `make` installed:

```
gcc -o sha1 main.c -O2
```

Run:

```
./sha1 [file_name]
```
