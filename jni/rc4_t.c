#include <stdlib.h>

#include "rc4_t.h"
#include "config.h"
#include "debug.h"

rc4_t* rc4_new(bs_t* key)
{
	rc4_t    *r;
	uint32_t i;
	uint8_t   j;
	byte_t   tmp;

	LOGPOS();

	if (key->len < 1 || key->len > 256) {
		LOGW("Error Key Size : %d.", key->len);
		return NULL;
	}
	
	r = (rc4_t*)malloc(sizeof(rc4_t));
	if (r == NULL) {
		LOGW("Error Memory Alloc.");
		return NULL;
	}
	
	for (i = 0; i < 256; i++) {
		r->s[i] = (byte_t)i;
	}
	
	j = 0;
	for (i = 0; i < 256; i++) {
		j += r->s[i] + key->data[i%key->len];
		tmp = r->s[i]; r->s[i] = r->s[j]; r->s[j] = tmp; 
	}

	for (i=0; i<256; ++i) {
		LOGI("0x%x", r->s[i]);
	}

	r->i = 0;
	r->j = 0;

	return r;
}

void rc4_xstream(rc4_t* r, bs_t *p, bs_t *c)
{
	uint32_t i;
	byte_t   T;
	uint8_t  I,J;
	byte_t   *S;

	S = r->s;
	I = r->i;
	J = r->j;

	LOGPOS();

	for (i = 0; i < p->len; i++) { 
		I += 1;
		J += S[I];
		T = S[I]; S[I] = S[J]; S[J] = T; 
		c->data[i] = p->data[i] ^ S[(uint8_t)(S[I]+S[J])];
	}

	r->i = I;
	r->j = J;
}

void rc4_reset(rc4_t* r)
{
	uint32_t i = 0;

	LOGPOS();

	for (i = 0; i < 256; i++) {
		r->s[i] = 0;
	} 
	r->i = r-> j = 0;
}

void rc4_destroy(rc4_t* r)
{
	LOGPOS();

	if (r != NULL) {
		free(r);
	}
}