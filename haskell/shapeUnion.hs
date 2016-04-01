-- shapeUnion.hs

-- create an alias
type Vector = (Double, Double)

-- create a new type, with two type constructors (algebraic data type)
data Shape = Circle Vector Double
           | Poly [Vector]
	   deriving (Show)