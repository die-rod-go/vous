var example;
//	initialize the list
var[] list : init_list;
list <- 0	: init_list;
list <- 7	: init_list;
list <- 2	: init_list;
list <- 5	: init_list;
list <- 11	: init_list;
list <- 9	: init_list;
list <- 10  : init_list;
list <- 255	: init_list;
var listSize = 8 : init_list;
emit init_list;

//	initialize loop variables
var currLargest = list[0] : init_loop_variables;
var index = 0			   : init_loop_variables;
emit init_loop_variables;

//	compare elements and find the largest
index = index + 1 : check_next_element;
currLargest = list[index] : found_larger;
emit found_larger ? list[index] > currLargest : check_next_element;
emit check_next_element ?? index + 1 < listSize;

//	print the largest element
print currLargest : print_largest;
emit print_largest;
