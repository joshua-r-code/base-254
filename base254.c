#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "base254.h"

// This is a library designed to convert a piece of binary data to a C-string compatible format.
// Here's how the string should be formatted
// 'b'|254|<NULL CHARACTER REPLACEMENT BYTE>|<ESCAPE BYTE>|<data>
// The null character replacement byte should be calculated based upon finding the least used byte in the data
// the ESCAPE BYTE should be the second least used byte in the data, or if the null character replacement byte
// appears zero times, the escape byte should be the same as the null replacement byte, which should lead to
// the decoder not interpreting it.

// Space complexity: (where N is the number of bytes of data)
// Best case: N+5
// Average case: N+5+(N/133)
// Worst case: N+5+(N/128)

static void* safe_malloc(size_t);

unsigned char* base254_encode(const void* dataOrig, size_t length) {
	unsigned char* data = (unsigned char*) dataOrig;
	size_t usage_count[256];
	for (int i=0; i < 256 ; i++) {
		usage_count[i] = 0;
	}
	for (size_t i=0; i < length ; i++) {
		int dataInd = data[i];
		usage_count[dataInd]++;
	}
	// find the char with the smallest usage count
	size_t minUse = (size_t)-1;  // minUse is set to the max of size_t
	unsigned char minUsedChar = 1;
	for (int i=1; i < 256 ; i++) {
		if (usage_count[i] < minUse) {
			minUse = usage_count[i];
			minUsedChar = (unsigned char) i;
		}
	}
	if (minUse == 0) {
		// if the null-replacement value is the same, we don't need
		// a separate escape value.
		return base254_encode_with_escapes(data, length, minUsedChar, minUsedChar);
	}
	unsigned char secondMinUsedChar = 1;
	size_t secondMinUse = (size_t)-1;
	for (int i=1; i < 256; i++) {
		if (i == ((int)minUsedChar)) {
			continue;
		}
		if (usage_count[i] < secondMinUse) {
			secondMinUse = usage_count[i];
			secondMinUsedChar = (unsigned char) i;
		}
	}
	return base254_encode_with_escapes(data, length, minUsedChar, secondMinUsedChar);
}

base254_data* base254_decode(const unsigned char* data) {
	return base254_decode_n(data, 0);
}

void free_base254_data(base254_data* d) {
	free(d->data);
	free(d);
}

base254_data* base254_decode_n(const unsigned char* data, size_t n) {
	if (n < 4 && n != 0) {
		return NULL;
	}
	if (!(data[0] == 'b' && data[1] == 254)) {
		printf("ERROR: not properly formatted base254 data.\n");
		return NULL;
	}
	unsigned char nullReplacement = data[2];
	unsigned char escapeByte = data[3];
	base254_data* bytes = safe_malloc(sizeof(base254_data));
	bool escape = false;
	size_t k = 0;
	size_t dataLen = n == 0 ? strlen(data)-4 : strnlen(data, n)-4;
	bytes->data = safe_malloc(dataLen);
	unsigned char* output = (unsigned char*) bytes->data;
	for (size_t i=4; (i < n|| n == 0) && data[i] != 0; i++) {
		if (escape) {
			output[k] = data[i];
			escape = false;
			k++;
		} else if (data[i] == escapeByte && escapeByte != nullReplacement) {
			escape = true;
		} else if (data[i] == nullReplacement) {
			output[k] = 0;
			k++;
		} else {
			output[k] = data[i];
			k++;
		}
	}
	bytes->size = k;
	return bytes;
}

unsigned char* base254_encode_with_escapes(const void* dataOrig, size_t length, unsigned char nullReplacement, unsigned char escapeByte) {
	const unsigned char* data = (unsigned char*) dataOrig;
	size_t requiredSize = 4;
	// first, calculate our required size.
	for (size_t i=0; i<length; i++) {
		if (data[i] == nullReplacement || data[i] == escapeByte) {  // if the character is literally
			// our null replacement, then we'll need a byte before to escape it
			requiredSize++;
		}
		requiredSize++;
	}
	requiredSize++;  // extra character for null byte at the end
	unsigned char* encoded = safe_malloc(requiredSize);
	size_t k = 0;
	encoded[k] = 'b';
	k++;
	encoded[k] = 254;
	k++;
	encoded[k] = nullReplacement;
	k++;
	encoded[k] = escapeByte;
	k++;
	for (size_t i = 0; i < length && k < requiredSize-1; i++) {
		if (data[i] == 0) {
			encoded[k] = nullReplacement;
			k++;
		} else if (data[i] == nullReplacement || data[i] == escapeByte) {
			encoded[k] = escapeByte;
			k++;
			encoded[k] = data[i];
			k++;
		} else {
			encoded[k] = data[i];
			k++;
		}
	}
	encoded[k] = 0;
	return encoded;
}

static void* safe_malloc(size_t len) {
	void* data = malloc(len);
	assert(data != NULL);
	memset(data, 0, len);
	return data;
}
