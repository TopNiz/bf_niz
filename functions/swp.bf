# Data stack: 10 bytes
#   varname mempos  Instruction
    A       00:     ()
    B       01:     >()
    C       02:     >()
    D       03:     >()
    E       04:     >()
    F       05:     >()
    G       06:     >()
    H       07:     >()
    I       08:     >()
    J       09:     >()
# Heap
# starts at 0A:

# Code
# A - B ==> A - 0 - B
# fncname   code                ; comment
#           0    0    1
#           0----5----0
            >                   ; start
    setA:   <<<<<<<<<<          ; goto A position
            +++++               ; set 5 to A
            >>>>>>>>>>          ; return to heap

    setB:   <<<<<<<<<           ; goto B position
            +++++++++           ; set 9 to B
            >>>>>>>>>           ; return to heap

    swpAB:  <<<<<<<<<<          ; goto A position
            >                   ; goto B to dup B
            [>+<-]              ; move B to C
            <                   ; goto A
            [>+<-]              ; move A to B
            >>                  ; goto C
            [<<+>>-]            ; move C to A
            >>>>>>>>            ; return to heap

    dispA:  <<<<<<<<<<          ; goto A
            .                   ; output A
            >>>>>>>>>>          ; return to heap

    dispB:  <<<<<<<<<           ; goto B
            .                   ; output B
            >>>>>>>>>           ; return to heap
