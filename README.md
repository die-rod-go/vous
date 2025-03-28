# Pulse
Pulse is an event-driven programming language where every statement is triggered by a named event. Execution follows a strict event subscription order, and control flow is managed through event calls rather than traditional loops or function calls.

Example:

```
//  two statements are created and subscribe to the init event.
var num1 = 1 : init;
var num2 = 1 : init;

//  the init event is called, creating num1 and num2
emit init;

//  a new statement is created and subscribes to the add event
var sum = num1 + num2 : add;

//  the add event is called and the two numbers are added together
emit add;
```

# Variables

*Declaration*
```
//  creates a variable named example
var example : event; // set to 0 by default
```

*Reassignment*
```
//  creates a variable named example initialized to 10
var example = 10 : event;
//  reassigns example to 5
example = 5 : event;
```
EVERY statement (sans emit statements) must be subscribed to an event.
```
//  WRONG
var example; <- [line 1] Error at ';': Expect ':' after variable declaration.
```

Variables can hold bytes or strings and can be reassigned.
```
var byteOrString : init;
emit init;

byteOrString = "Hello World!" : set_to_string;
print byteOrString : set_to_string;
emit set_to_String;

byteOrString = 12 : set_to_string;
print byteOrString : set_to_string;
emit set_to_String;

output:
Hello World!
1
```


# Arrays
Pulse supports dynamic lists which allow storing multiple values.

*Declaration*
```
// Creates an empty list
var[] arr : event;
```

*Adding Elements*
```
// Pushes values into the list
arr <- 3 : event;
arr <- "test" : event;
```

Note that lists can store heterogenous data.

*Accessing and Modifying Elements*
```
// Access an element
var x = arr[0] : event; // Gets the first element (3)

// Modify an element
arr[1] = 5 : event; // Sets the second element to 5
```

# Events

There's three ways to trigger events.

*Once, unconditionally*
```
emit event_name;
```
Usage:
```
print 1 : print_event;
emit print_event;

output:
1
```

*Once, conditionally*

```
emit event_name ? condition;
```

Usage:
```
var num = 10 : init;
emit init;

print "Greater than five!" : print_event;
emit print_event ? num > 5;

output:
Greater than five!
```

*Looping, conditionally*
```
emit event_name ?? condition;
```

Usage:
```
var num = 10 : init;
emit init;

print 1 : print_event;
emit print_event ?? num > 5;

output:
1
1
1
1
...
```
```
//  infinite loop
emit event ?? 1;
```

# Print
Does what it says on the tin.

Usage:
```
print "I'm printing!" : print_event;
emit print_event;

output:
I'm printing!
```
*Printing variables*
```
var x = 5 : init;
print x : init;
emit init;

output:
5
```

# Event chaining
Emit statements can also be subscribed to events. 
```
//  event_name will NOT be emitted by default. it will be emitted for the first time when subscribed_event is emitted 
emit event_name : subscribed_event;

//  works with conditionals
emit event_name ? condition : subscribed_event;

// works with looping conditionals
emit event_name ?? condition : subcribed_event;
```
Take this code for a binary search for example.
```
//  initialize implementation variables
var secretNumber = 33 : init;
var numGuesses = 0 : init;
var low = 0 : init;
var high = 255 : init;
var numGuessed : init;
emit init;

numGuessed = (high + low) / 2 : guess;
numGuesses = numGuesses + 1 : guess;

low = numGuessed : guessed_too_high;
high = numGuessed : guessed_too_low;

//  the guessed_too_high and guessed_too_low events will be emitted every time the guess event is emitted
emit guessed_too_high ? numGuessed > secretNumber : guess;
emit guessed_too_low ? numGuessed < secretNumber : guess;

//  updates numGuessed, numGuessed, AND low and high since their event
//  (guessed_too_high and guessed_too_low are also subscribed to the guess event)
emit guess ?? numGuessed != secretNumber;

print numGuessed : done;
print numGuesses : done;
emit done;
```

# Initialization Order
Event chaining can cause confusing initialization order errors. For example, look at this code.
```
var test = 10 : init;
emit init ? test == 10 : chain; // [line 2] Error: Undefined variable 'test'.
emit chain;
```
'emit chain' calls 'emit init', which has a conditional that relies on the variable 'test' which is only initialized after init is called once. So when 'test == 10'  is checked, test does not exist. It's fairly easy to see the error here, but with longer event chains, it can get a little confusing.
