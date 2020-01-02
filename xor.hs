import Control.Monad (zipWithM)
import Data.Bits (xor)
import qualified Data.ByteString.Lazy as B
import System.Environment (getArgs)

-- compile with: ghc xor.hs
main :: IO ()
main = getArgs >>= parse

usage :: IO ()
usage =
  putStrLn
    "Trivial secret sharing between n parties so that all n pieces are needed for reconstruction\n\
    \(see https://en.wikipedia.org/wiki/Secret_sharing#t_=_n).\n\n\
    \usage: ./xor [ out <numoutfiles> | in <in1> <in2> ... ]\n\n\
            \XOR for multiple streams.\n\n\
            \out:\tSplit up the input read from stdin into <numoutfiles> files\n\
            \\twhich XORed together result in the original input file (exactly all are needed).\n\
            \\tThis uses numoutfiles-1 random bytestreams of the same length as the input\n\
            \\tand XORes them together.\n\n\
            \in:\tCombine/XOR each input file together to reconstruct the original file\n\
            \\twhich is then printed to stdout.\n\n\n\
            \Example calls:  Split up some file into five parts out1, out2, out3, out4, out5:\n\n\
            \\techo 'Hello-World' | ./xor out 5\n\n\
            \Reconstruct the original file from the five parts (all five are needed),\n\
            \each has to be specified exactly once, the order doesn't matter because\n\
            \XOR is commutative.\n\n\
            \\t./xor in out1 out3 out2 out4 out5\n\
            \\t# Hello-World"

-- core functionality (xoring of files) adapted from https://gist.github.com/ivanperez-keera/418afd08dbd90b4216cd
parse ("in":args) = do
  if length args < 2
    then usage
    else do
      filebytes <- mapM B.readFile args
      let res = foldr1 (\x y -> B.pack $ B.zipWith xor x y) filebytes
      B.putStr res
parse ["out", split] = do
  let splitnum = read split :: Int
  if splitnum < 2
    then usage
    else do
      input <- B.getContents
      temp <- mapM B.readFile $ replicate (splitnum - 1) "/dev/urandom"
      let rand = map (B.take (B.length input)) temp
          names = map (("out" ++) . show) [1 .. splitnum]
          res = foldr1 (\x y -> B.pack $ B.zipWith xor x y) (input : rand)
      zipWithM B.writeFile names (res : rand)
      return ()
parse other = usage
