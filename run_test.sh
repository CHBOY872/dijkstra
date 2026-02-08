#!/bin/sh

while IFS= read -r line; do
    echo "===== TEST ====="
    echo "Run test case with this input: $line"
    echo "$line" | ./main | tail -n 1 | grep -v Type # If the path was not
                                                     # displayed, it means
                                                     # that it is impossible
                                                     # to construct a path
                                                     # from the current graph
                                                     # based on the specified
                                                     # values.
done < test_cases.txt

