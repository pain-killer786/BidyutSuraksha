/* Helper.h */

#ifndef helpers_h
#define helpers_h

/* 
    Flush buffer after user enter some input
    If we don't flush buffer from 'enter' pressed by user will be stroed in buffer,
    and it will be used as an input to next I/O functions, which may cause issue. 
*/

extern void flush_buffer(void);
extern  char * tokenize_string(char * source, char const * delimiter, char ** context);

#endif /* helpers_h */

