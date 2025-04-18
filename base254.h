#ifndef BASE254_H
#define BASE254_H


unsigned char* base254_encode(const void*, size_t);
unsigned char* base254_encode_with_escapes(const void*, size_t, unsigned char, unsigned char);
typedef struct {
	void* data;
	size_t size;
} base254_data;
void free_base254_data(base254_data*);
base254_data* base254_decode(const unsigned char*);
base254_data* base254_decode_n(const unsigned char*, size_t);
#endif
