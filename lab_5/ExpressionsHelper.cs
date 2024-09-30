using static Interpreter.Tokenizer;

namespace Interpreter
{
    public class ExpressionsHelper
    {
        public static void CheckStack(Interpreter.Context context)
        {
            if (context.Tokens.Count <= 0)
                throw new Exception("Неизвестный символ.");
        }

        public static Exception ThrowUnexpectedToken(Token token)
        {
            return new Exception($"Неизвестный символ {token.TokenString}.");
        }
    }
}
