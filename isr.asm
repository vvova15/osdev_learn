bits 32

global isr0
global isr33
global isr32

extern isr0_handler
extern timer_handler
extern keyboard_handler

isr0:
    pushad
    call isr0_handler
    popad
    iretd

isr32:
    pushad
    call timer_handler
    popad
    iretd

isr33:
    pushad
    call keyboard_handler
    popad
    iretd