option casemap :none

_TEXT    SEGMENT
    ;https://msdn.microsoft.com/en-us/library/windows/hardware/ff561499(v=vs.85).aspx

    ;mangled functions
    EXTERN objectDraw:               PROC;    EngineHook::_scriptEntered

    ;JmpBacks

    EXTERN ObjectForDrawingJmpBack:                           qword
    EXTERN ObjectForDrawingEndJmpBack:                           qword

    ;misc
    EXTERN forceLod:                                    dword
    EXTERN objPtr:                                    qword
    ;EXTERN evalIngress:                                    qword

    PUBLIC objectDrawHook
    objectDrawHook PROC

        ;object rcx
        ;forcelod r9d


        mov forceLod, r9d
        mov objPtr, rdx

        push rax ;hold
        push rcx



        call    objectDraw;        ObjDraw
       ; mov r9d, forceLod 
        pop rcx;restore
        pop rax



        mov     [rsp+20h], r9d
        mov     [rsp+18h], r8d
        mov     [rsp+8], rcx





    ;Fixup
        push    rbp
        push    rsi
        push    rdi
        push    r14
        push    r15
        lea     rbp, [rsp-0C0h]
        sub     rsp, 1C0h

        jmp ObjectForDrawingJmpBack;

    objectDrawHook ENDP

    PUBLIC objectDrawHookEnd
    objectDrawHookEnd PROC

    
        mov     eax, forceLod

        mov     [rbx+0BEh], al


        mov     eax, [rbp+108h]

        mov     [rbx+0BFh], al
        movzx   ecx, byte ptr [r15+9E0h]
        xor     eax, eax
        mov     [rbx+0BAh], cl


        jmp ObjectForDrawingEndJmpBack;

    objectDrawHookEnd ENDP


    PUBLIC evalIngressFnc
    evalIngressFnc PROC
        mov     r9, rax
        ;jmp evalIngress;
    evalIngressFnc ENDP




_TEXT    ENDS
END
