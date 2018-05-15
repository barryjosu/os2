A two part assignment. Part 1 is to implement a mutual-exclusion relationship where up to three processes can use a resource, and all three must stop before a new process can if three end up using it at once. Part 2 is to implement a singly-linked list that can be shared by searchers, inserters, and deleters with a few constraints.

`make` can be run to compile both parts.
`./problem1` will run the solution to part 1.
Part 2 takes some command line arguments, and `./problem2 <numsearchers> <numdeleters> <numinserters>` is the template.