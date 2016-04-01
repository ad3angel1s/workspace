-- listLength.hs

listLength (x:xs) = 1 + listLength xs
listLength []     = 0

-- signature

listLength :: [t] -> Float

-- sumList

sumList (x:xs) = x + sumList xs
sumList [] = 0

meanList (x:xs) = s/l
                  where l = listLength (x:xs)
		        s = sumList (x:xs)
meanList []     = 0