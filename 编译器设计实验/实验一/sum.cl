class Main inherits IO {
    sum: Int <- 0;
    i: Int <- 1;
    main(): SELF_TYPE { {
        while i <= 100 loop {
            sum <- sum + i;
            i <- i + 1;
        } pool;
        out_string("sum from 1 to 100: ");
        out_int(sum);
        out_string("\n");
    } };
};