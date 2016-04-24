def separate( string, values ):
    if len(values)==1:
        if string=="":
            print str(values[0])
        else:
            print string + "+" + str(values[0])
            print string + "-" + str(values[0])
    else:
        separate( string + str(values[0]), values[1:] )

        if len(values)>=2:
            separate( string, [str(values[0]) + str(values[1])] + values[2:] )

    return

# Exercise
separate( "", [1,2,3] )
