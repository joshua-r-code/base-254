# Base254

## Purpose
This is a library designed to convert binary data to C-strings, with a null terminating byte at the end.

## Usage
Place the `base254.c` and `base254.h` in the same directory as your C project. Then, include the files using `#include "base254.h"` in your header files, or at the top
of your `.c` files.
If using `gcc`, be sure that your base254.c file is included in your arguments, for example `gcc -o out *.c`, which, on most shells will include
all files in the present working directory, alternatively you can specify it individually, `gcc -o out main.c base254.c`. After which the functions should be accessible
within your program.

## Storage efficiency (where N represents the number of bytes of the original data)
| Best | Average | Worst |
|-------------|-------------|-------------|
|N+5|N+5+(N/133)|N+5+(N/128)|

## Byte structure and format
Say we have the following five bytes of data: {0x0, 0x1, 0x1, 0x1, 0x1}
If we were to encoded it with this library using...
```
unsigned char bytes[] = {0, 1, 1, 1, 1};
unsigned char* convertedInfo = base254_encode(bytes, 5);
```
The variable `convertedInfo` would contain the following data
`{98, 254, 2, 2, 2, 1, 1, 1, 1, 0}`

Let's break it down byte by byte

1. (First byte) 98 is the ascii code for "b", 
2. 254 is a literal 254 as an unsigned char, these first two bytes identify a string as being in base254 format.
3. The next byte (2) specifies the null replacement value. Any occurance of `2` in the string is interpreted by the decoder as being a 0 in the original binary data.
4. The next byte (2 again) represents the escape value. In this case, it is the exact same as the null replacement value. This means that there is no escape value, as an escape sequence is only necessary when there are occurances of the literal null replacement value within the binary data. 
5. The byte after that is the first byte of the original binary data, it is a 2, which is replaced by the decoder as a 0.
6. The next four bytes are literal 1's
7. ^^
8. ^^
9. ^^
10. The tenth and final byte is a 0, which is the null-terminator in a C string


## Functions
`unsigned char* base254_encode(const void*, size_t)`
Example usage: 
```
#include "base254.h"

int main() {
    char* encodedData = base254_encode("Hello world", 12);
    free(encodedData); // must free data when finished with it
    return 0;
}
```
It goes without saying that any data, not just strings, could be passed into `base254_encode`, the size\_t value is the size of the binary data in bytes. Note that, while you could (in theory) pass an `int` pointer, it is generally unwise, as the result will depend entirely on the endianess of your CPU architecture, and not be portable.

`base254_data* base254_decode(const unsigned char*);`

Example usage:
```
#include <stdio.h>
#include "base254.h"

int main() {
    char* encodedData = base254_encode("Hello world", 12);
    base254_data* bData = base254_decode(encodedData);
    for (size_t i=0; i < bData->size ; i++) {
        putchar(((char*)bData->data)[i]);  // prints out each character individually
    }
    free(encodedData);
    free_base254_data(bData);
}
```

`base254_data* base254_decode_n(const unsigned char*, size_t);`

Similar to the above function, except the length of the string is limited by the second argument to the function. 

```
#include <stdio.h>
#include "base254.h"

int main() {
    char* encodedData = base254_encode("Hello world", 12);
    base254_data* bData = base254_decode_n(encodedData, 16);
    for (size_t i=0; i < bData->size ; i++) {
        putchar(((char*) bData->data)[i]);  // prints out each character individually
    }
    free_base254_data(bData);
    free(encodedData);
    return 0;
}
```

## Structures
```
typedef struct {
    void* data;
    size_t size;
} base254_data;
```
This is the primary data structure for returning data after decoding it. `size` specifies how many bytes long the data is, it can be accessed as shown in the examples.
`data` is a pointer to the binary data itself 
