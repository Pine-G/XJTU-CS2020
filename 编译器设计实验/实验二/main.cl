class Main inherits IO {

    stack : List;
    
    newline() : Object { out_string("\n") };
    
    prompt() : String { { out_string(">"); in_string(); } };
    
    display_stack(s : List) : Object {
        if s.isNil() then out_string("") else {
            out_string(s.head());
            out_string("\n");
            display_stack(s.tail());
            }
        fi
    };
    
    main() : Object {
        (let z: A2I <- new A2I, stack: List <- new List, flag: Bool <- true in
            while flag loop
                (let s : String <- prompt() in
                    if s = "x" then flag <- false else
                    if s = "d" then display_stack(stack) else
                    if s = "e" then {
                        if stack.isNil() then out_string("") else
                        if stack.head() = "+" then {
                            stack <- stack.tail();
                            (let a : Int <- new Int, b : Int <- new Int in {
                                a <- z.a2i(stack.head());
                                stack <- stack.tail();
                                b <- z.a2i(stack.head());
                                stack <- stack.tail();
                                a <- a + b;
                                stack <- stack.cons(z.i2a(a));
                                }
                            );
                        } else
                        if stack.head() = "s" then {
                            stack <- stack.tail();
                            (let a : String <- new String, b : String <- new String in {
                                a <- stack.head();
                                stack <- stack.tail();
                                b <- stack.head();
                                stack <- stack.tail();
                                stack <- stack.cons(a);
                                stack <- stack.cons(b);
                                }
                            );
                        } else out_string("")
                        fi fi fi;
                    } else stack <- stack.cons(s)
                    fi fi fi
                )
            pool
        )
    };

};