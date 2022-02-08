# Fast Prime Calculator
## Persuit for the fastest prime number calculations


Just a small personal project for the fastest prime number calculations, using all methods that I know of.


### Optimizations used

- Mutexes for thread safe objects
- SSE instructions (specifically AVX instruction set)

### Compile
It will compile with level 3 optimization and -mavx2 parameters
```sh
make all
```
### Run
Reccomended to run with a processor which supports at least an AVX2 instruction set
```sh
./build/prime
```
