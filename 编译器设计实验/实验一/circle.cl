class Main inherits IO {
    main(): SELF_TYPE { {
        out_string("input radius: ");
        let r:Int <- in_int(), area:Int in {
            area <- 314 * r * r;
            out_int(area / 100);
            out_string(".");
            out_int(area - area / 100 * 100);
            out_string("\n");
        };
    } };
};