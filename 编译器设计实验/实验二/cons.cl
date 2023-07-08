-- Cons class implements the non-empty list

class Cons inherits List {

    first : String;
    rest : List;
    
    isNil() : Bool { false };

    head() : String { first };

    tail() : List { rest };

    init(head : String, next : List) : List {
        {
            first <- head;
            rest <- next;
            self;
        }
    };
};