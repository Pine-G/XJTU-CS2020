-- List class implements the empty list

class List {

    isNil() : Bool { true };
    
    head() : String { { abort(); ""; } };
    
    tail() : List { { abort(); self; } };

    cons(i : String) : List { (new Cons).init(i, self) };

};