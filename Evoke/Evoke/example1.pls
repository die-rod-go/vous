byte[] arr : init_arr;
arr <- 3  : init_arr;
arr <- 10 : init_arr;
arr <- 15 : init_arr;
arr <- 20 : init_arr;
arr <- 1  : init_arr;
emit init_arr;

byte inner = 0 : init_loop;
byte outer = 5 : init_loop;
emit init_loop;

byte temp = arr[inner] : swap;
arr[inner] = arr[inner + 1] : swap;
arr[inner + 1] = temp : swap;

inner = inner + 1 : increment_inner;
outer = outer - 1 : decrement_outer;

inner = 0 : reset_inner;
emit reset_inner ? inner >= outer : increment_inner;

emit swap ? arr[inner] > arr[inner + 1] : loop;
emit increment_inner : loop;
emit decrement_outer : loop;
emit loop ?? outer > 0;

