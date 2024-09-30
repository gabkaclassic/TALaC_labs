using static Interpreter.Tokenizer;

namespace Interpreter
{
    public class Term
    {
        public static int Parse(Interpreter.Context context)
        {
            var _context = context;
            var _stack = context.Tokens;
            var factorResult = Factor.Parse(context);

            if (_stack.Count <= 0)
                return factorResult;

            var @operator = _stack.Peek();
            if (@operator.Type == TokenType.Operator)
            {
                if (@operator.TokenString == "*")
                {
                    _stack.Pop();
                    return factorResult * Parse(_context);
                }
                else if (@operator.TokenString == "/")
                {
                    _stack.Pop();
                    return factorResult / Parse(_context);
                }
            }
            return factorResult;
        }
    }
}
