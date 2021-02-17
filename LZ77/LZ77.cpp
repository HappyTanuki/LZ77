#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct _LLD{
	int distance;
	int length;
	unsigned char ltl;
	_LLD* next;
}LLD;

int* getPi(unsigned char* substring, int substringSize);
unsigned char* kmpSearch(unsigned char* string, int stringSize, unsigned char* substring, int substringSize);
unsigned char* lz77compress(unsigned char* ori_data, int dataSize, int windowBufferSize, int lookAheadBufferSize, unsigned char *compressedData);

int* getPi(unsigned char* substring, int substringSize) {
	int i, j;
	int* pi = (int*)malloc(substringSize * sizeof(int));
	unsigned char* tmp = (unsigned char*)malloc(substringSize * sizeof(unsigned char));
	unsigned char* pre = (unsigned char*)malloc(substringSize * sizeof(unsigned char));
	unsigned char* suf = (unsigned char*)malloc(substringSize * sizeof(unsigned char));

	if (pi == NULL || pre == NULL || suf == NULL || tmp == NULL)
		return NULL;

	memset(tmp, 0, substringSize * sizeof(unsigned char));
	memset(pi, 0, substringSize * sizeof(int));

	for (i = 0; i < substringSize; i++) {
		memcpy(tmp, substring, i + 1);
		for (j = i; j > 0; j--) {
			memset(pre, 0, substringSize * sizeof(unsigned char));
			memcpy(pre, substring, j);
			memset(suf, 0, substringSize * sizeof(unsigned char));
			memcpy(suf, substring + i - j + 1, j);
			if (!memcmp(pre, suf, j))
				break;
		}

		pi[i] = j;
	}

	free(tmp);
	free(pre);
	free(suf);

	return pi;
}

unsigned char* kmpSearch(unsigned char* string, int stringSize, unsigned char* substring, int substringSize) {
	int i, j;
	int* pi = getPi(substring, substringSize);

	if (pi == NULL)
		return NULL;

	j = 0;
	for (i = 0; i < stringSize; i++) {
		while (j > 0 && string[i] != substring[j])
			j = pi[j - 1];
		if (string[i] == substring[j]) {
			if (j == substringSize - 1) {
				free(pi);
				return string + i - substringSize + 1;
				j = pi[j];
			}
			else
				j++;
		}
	}

	return NULL;
}

unsigned char* lz77compress(unsigned char* ori_data, int dataSize, int windowBufferSize, int lookAheadBufferSize, unsigned char* compressedData) {
	LLD*		   lldp = (LLD*)malloc(sizeof(LLD));
	LLD* lldhead = lldp;
	unsigned char* wndbuf = (unsigned char*)malloc(windowBufferSize * sizeof(unsigned char));
	unsigned char* lahbuf = (unsigned char*)malloc(lookAheadBufferSize * sizeof(unsigned char));
	unsigned char* data = ori_data;
	unsigned char* searchbuf;
	unsigned char* tmp;
	int i;

	searchbuf = (unsigned char*)malloc(lookAheadBufferSize * sizeof(unsigned char));
	if (wndbuf == NULL || lahbuf == NULL || searchbuf == NULL || lldp == NULL) {
		free(lldp);
		free(wndbuf);
		free(lahbuf);
		free(searchbuf);
		return NULL;
	}

	memset(wndbuf, 0, windowBufferSize * sizeof(unsigned char));
	while (data < ori_data + dataSize) {
		memset(lahbuf, 0, lookAheadBufferSize);
		memcpy(lahbuf, data, lookAheadBufferSize);
		memcpy(wndbuf, ori_data, data - ori_data);
		for (i = lookAheadBufferSize; i >= 0; i--) {
			memset(searchbuf, 0, lookAheadBufferSize * sizeof(unsigned char));
			memcpy(searchbuf, lahbuf, i);
			tmp = kmpSearch(wndbuf, windowBufferSize, searchbuf, i);
			if (tmp) {
				lldp->distance = (data - ori_data) - (tmp - wndbuf);
				lldp->length = i;
				lldp->ltl = *(data + i);
				lldp->next = (LLD*)malloc(sizeof(LLD));
				if (lldp->next == NULL) {
					return NULL;
				}
				lldp = lldp->next;
				lldp->next = NULL;
				data += i + 1;
				break;
			}
			else if (i == 0) {
				lldp->distance = (data - ori_data) - (tmp - wndbuf);
				lldp->length = 0;
				lldp->ltl = *(data);
				lldp->next = (LLD*)malloc(sizeof(LLD));
				if (lldp->next == NULL) {
					return NULL;
				}
				lldp = lldp->next;
				lldp->next = NULL;
				data++;
				break;
			}
		}
	}
	compressedData = (unsigned char*)malloc(1000 * sizeof(unsigned char));
	memset(compressedData, 0, 1000 * sizeof(unsigned char));
	tmp = (unsigned char*)malloc(1000 * sizeof(unsigned char));
	unsigned char* tmpp = compressedData;
	lldp = lldhead;
	while (true) {
		if (!lldp->next)
			break;
		sprintf((char*)tmp, "(%d,%d,%c)", lldp->distance, lldp->length, lldp->ltl);
		memcpy(tmpp, tmp, strlen((char *)tmp));
		tmpp += strlen((char*)tmp);
		lldp = lldp->next;
	}

	return NULL;
}

int main() {
	unsigned char a[] = "AOHLBOHLCOHL";
	unsigned char* b = NULL;

	lz77compress(a, 12, 6, 4, b);
	fprintf(stdout, "%s", b);
	
	return 0;
}