        .segment "HEADER"
        .byte "NES", $1A
        .byte $02        ; 2 x 16KB PRG
        .byte $01        ; 1 x 8KB CHR
        .byte $00        ; mapper 0
        .byte $00
        .byte $00,$00,$00,$00,$00,$00,$00,$00
