#ifndef __SKLAFFTERM_UI_H__
#define __SKLAFFTERM_UI_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void sklaffterm_keyboard_handler(const int keystroke,
                                        const int flags);
extern void sklaffterm_refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* __SKLAFFTERM_UI_H__ */
