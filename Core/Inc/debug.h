/**
 * Функции отладочного вывода.
 *
 * Code style специально отличается от основного в текущем слое программы,
 * чтобы выделяться на фоне вызывающего кода.
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG
void debug_logs(const char *str);
void debug_logi(int n);
void debug_logx(int n);
#else
static inline void debug_logs(const char *str) { }
static inline void debug_logi(int n) { }
static inline void debug_logx(int n) { }
#endif

#endif /* _DEBUG_H */
