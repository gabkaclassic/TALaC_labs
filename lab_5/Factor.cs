using static Interpreter.Tokenizer;

namespace Interpreter
{
    public class Factor
    {
        public static int Parse(Interpreter.Context context)
        {
            var _context = context;
            var _stack = context.Tokens;
            var _table = context.Variables;
            var token = _stack.Pop();
            var _type = token.Type;
            var _string = token.TokenString;
            if (_type == TokenType.Digit)
            {
                return Convert.ToInt32(_string);
            }

            if (_type == TokenType.Character)
            {
                var isInTable = _table.ContainsKey(_string);
                if (!isInTable)
                    throw new Exception($"Неизвестный токен {token.TokenString}.");
                return _table[_string];
            }

            if (_type == TokenType.Bracket && _string == "(")
            {
                var result = Expression.Parse(_context);
                ExpressionsHelper.CheckStack(_context);
                var closeBracket = _stack.Pop();
                if (closeBracket.TokenString == ")")
                    return result;
            }
            throw new Exception($"Неизвестный символ {_string}.");
        }
    }
}
