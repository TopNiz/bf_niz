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
# fncname   code                ; comment
            >                   ; start
    setA:   <<<<<<<<<<          ; goto A position
            +++++               ; set 5 to A
            >>>>>>>>>>          ; return to heap

    dupA:   <<<<<<<<<<          ; goto A position
            [                   ; start loop
             >+                 ; shift & incr B
             >+                 ; shift & incr C
             <<-                ; go back decr A
            ]                   ; end loop
            >>                  ; shift to C
            [                   ;
             <<+                ; incr A
             >>-                ; decr C
            ]                   ;
            >>>>>>>>            ; return to heap

    dispA:  <<<<<<<<<<          ; goto A
            .                   ; output A
            >>>>>>>>>>          ; return to heap

    dispB:  <<<<<<<<<           ; goto B
            .                   ; output B
            >>>>>>>>>           ; return to heap
