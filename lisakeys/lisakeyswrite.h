#ifndef LISAKEYSWRITE_H
#define LISAKEYSWRITE_H

void lk_write_raw(unsigned char * s);
void lk_write_hex(unsigned char * s);
void lk_write_ascii(unsigned char * s);
void lk_write_coded(unsigned char * s);
void lk_write_blu_bootstrap(unsigned char s);

#endif
