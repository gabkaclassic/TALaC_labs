using static Interpreter.Tokenizer;

namespace Interpreter
{
    public class Expression
    {
        public static int Parse(Interpreter.Context context)
        {
            var _context = context;
            var _stack = context.Tokens;
            var termResult = Term.Parse(_context);

            if (_stack.Count <= 0)
                return termResult;

            var @operator = _stack.Peek();
            if (@operator.Type == TokenType.Operator)
            {
                if (@operator.TokenString == "+")
                {
                    _stack.Pop();
                    return termResult + Parse(_context);
                }
                else if (@operator.TokenString == "-")
                {
                    _stack.Pop();
                    if (_stack.Count > 1)
                    {
                        var next = _stack.Pop();
                        var nextOperation = _stack.Peek();
                        _stack.Push(next);
                        bool isNextMultOperation = nextOperation.TokenString == "*" || nextOperation.TokenString == "/";
                        if ((next.Type == TokenType.Digit || next.Type == TokenType.Character) && !isNextMultOperation)
                        {
                            var result = termResult - Factor.Parse(_context);
                            var newToken = new Token(result.ToString(), TokenType.Digit);
                            _stack.Push(newToken);
                            return Parse(_context);
                        }
                    }
                    return termResult - Parse(_context);
                }
            }
            return termResult;
        }
    }
}
