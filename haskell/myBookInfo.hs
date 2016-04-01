-- myBookInfo.hs
-- data <type constructor> = <value constructor> <structure>
-- it will be then referenced by the <value constructor> in expressions
-- and by <type constructor> in signatures

data BookInfo = Book Int String [String]
                deriving (Show)

-- use to type to create alias

type AliasBookInfo = BookInfo

-- pattern matching as assessor function (function names must start with lowercase)
bookId      (Book id title authors) = id
bookTitle   (Book id title authors) = title
bookAuthors (Book id title authors) = authors

-- we could use the wild card character _ if we don't want to have exact match
-- alternatively we could use this more compact notation for the type definition

data StudentInfo = Student
                   {
                     studentName    :: String,
		     studentSurname :: String,
		     studentAge     :: Int
		   }
		   deriving (Show)

-- this notation will also create automatically accessors e.g. studentName

-- we could do something like this to avoid full pattern matching:
--
--  mySumList (x:xs) = x + mySumList xs
--  mySumList _      = 0
--
-- the second line is valid for all other cases
