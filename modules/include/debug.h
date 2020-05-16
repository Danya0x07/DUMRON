/**
 * Функции отладочного вывода.
 *
 * Code style специально отличается от основного в текущем слое программы,
 * чтобы выделяться на фоне вызывающего кода.
 */
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#ifdef DEBUG
void debug_logs(char *);
void debug_logi(int);
void debug_logx(int);
#else
static inline void debug_logs(char *str) { }
static inline void debug_logi(int n) { }
static inline void debug_logx(int n) { }
#endif

#endif
