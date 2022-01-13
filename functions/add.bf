# Data stack: 10 bytes
#   varname mempos  Instruction
    A       00:     ()
    B       01:     >()
    C       02:     >()
            03:     >()
            04:     >()
            05:     >()
            06:     >()
            07:     >()
            08:     >()
            09:     >()
# Heap
# starts at 0A:

# Code
# fncname   code                ; comment
            >                   ; start
    setA:   <<<<<<<<<<          ; goto A position
            +++++               ; set 5 to A
            >>>>>>>>>>          ; return to heap

    setB:   <<<<<<<<<           ; goto B position
            +++                 ; set 3 to B
            >>>>>>>>>           ; return to heap

    addAB:  <<<<<<<<<<          ; goto A position
            [                   ; start loop
             >                  ; shift to B
             +                  ; increment B
             <                  ; shift bakc to A
             -                  ; decr A
            ]                   ; end loop
            >>>>>>>>>>          ; return to heap

    dispB:  <<<<<<<<<           ; goto B
            .                   ; output B
            >>>>>>>>>           ; return to heap
