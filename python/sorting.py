def naive_sort( input ):
    output = []
    while input:
        print output+input
        min_val, min_idx = min( (min_val, min_idx) for (min_idx, min_val) in enumerate(input) )
        tmp = input[0]
        input[0] = input[min_idx]
        input[min_idx] = tmp
        output.append(input.pop(0))
    return output

def bubble_sort( input ):
    output = []
    swapped = True
    length = len(input)
    while swapped:
        swapped = False
        print input
        for idx in xrange(0,length-1):
            if input[idx] > input[idx+1]:
                tmp = input[idx]
                input[idx] = input[idx+1]
                input[idx+1] = tmp
                swapped = True
    return input

print "Naive Sort"
print naive_sort([5, 4, 3, 2, 1])

print "\nBubble Sort"
print bubble_sort([5, 4, 3, 2, 1])
