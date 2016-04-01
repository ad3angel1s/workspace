-- myDrop.hs

-- signature
myDrop :: Int -> [a] -> [a]

-- bind
myDrop n xs = if n <= 0 || null xs
              then xs
	      else myDrop (n-1) (tail xs)