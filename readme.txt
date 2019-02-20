1. For the question 1 (and the ensuing one) there is at any moment a single 
token per communicator. A process cannot just create tokens, it should first 
receive the token from their predecessor before being able to send it to their 
successor. Obviously, this order should be different for the initiator, aka.
the process that has the original token.

2. Taking in account that the matrix is NxN, to make the homework simpler 
let's go with something that the data distribution is block/block.

The size of data matrix N should be also part of the function argument.
You will need a pointer to the data, 2 int for P and Q, and the size of
the matrix, these are arguments for the transpose_matrix function you 
are supposed to write as part of the homework.
