-- add.hs

add a b = a + b

-- use the list construction operator, :
-- e.g. [1,2,3] is the same as 1:(2:(3:[]))
-- with pattern matching x:xs will be matched x to 1, and xs to (2:(3:[]))

addList (x:xs) = x + addList xs
addList [] = 0