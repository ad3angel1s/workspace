-- nullable.hs

-- introducing parametrised type
-- here 'a' represents a type variable

data Maybe a = ReallyJust a
             | NothingAtAll
	     deriving (Show)
	     
-- this type has two type constructors, one which uses the additional type variable, the other one doesn't use it

-- introducing recursive types

data MyList a = Ctor a (MyList a)
              | Nil
	      deriving (Show)

-- MyList appears on both sides, so to create a new type MyList I need to provide a MyList variable, and I know that Nil is of type MyList

-- define a function to deconstruct the MyList parametrised type

fromMyList (x:xs) = Ctor x (fromMyList xs)
fromMyList []     = Nil 

-- this function when applied to a classicly generated List object will show
-- the same output of the constructed object of MyList type

-- create a function which decomposes a MyList a into [a]

splitMyList (Ctor x xs) = x:(splitMyList xs)
splitMyList Nil         = []