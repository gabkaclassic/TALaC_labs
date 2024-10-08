﻿using static Interpreter.Tokenizer;

namespace Interpreter
{
    public class Interpreter
    {
        public void Interpret(string code)
        {
            var tokinizer = new Tokenizer();
            var tokens = tokinizer.Tokenize(code);
            var onlyTokens = tokens.Where(x => x.Type != TokenType.Space);
            var context = new Context(onlyTokens);
            Interpret(context);
        }

        private void Interpret(Context context)
        {
            try
            {
                while (context.Tokens.Count > 0)
                    Statement(context);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void Statement(Context context)
        {
            if (context.Tokens.Count == 0)
                return;
            var tokenFull = context.Tokens.Pop();
            var type = tokenFull.Type;
            var token = tokenFull.TokenString;
            if (token == "if")
            {
                If(context);
            }
            else if (token == "for")
            {
                For(context);
            }
            else if (token == "print")
            {
                Print(context);
            }
            else if (token == "scan")
            {
                Scan(context);
            }
            else if (type == TokenType.Character)
            {
                context.Tokens.Push(tokenFull);
                Assign(context);
            }
            else
                throw ExpressionsHelper.ThrowUnexpectedToken(tokenFull);
        }

        private void Assign(Context context)
        {
            ExpressionsHelper.CheckStack(context);
            var identifier = context.Tokens.Pop();
            var equalOperator = context.Tokens.Pop();
            if (equalOperator.Type != TokenType.Equal)
                throw ExpressionsHelper.ThrowUnexpectedToken(equalOperator);
            var expression = Expression.Parse(context);
            context.Variables[identifier.TokenString] = expression;
        }

        private void Scan(Context context)
        {
            ExpressionsHelper.CheckStack(context);
            var token = context.Tokens.Pop();
            if (token.Type != TokenType.Character)
                throw new Exception($"Невалидный символ для \"scan\" {token.TokenString}.");
            var value = int.Parse(Console.ReadLine());
            context.Variables[token.TokenString] = value;
            ExpressionsHelper.CheckStack(context);
            token = context.Tokens.Pop();
            if (token.TokenString != ";")
                throw new Exception($"Невалидный символ для \"scan\" {token.TokenString}.");
        }

        private void Print(Context context)
        {
            var result = PrintEnd(context);
            Console.WriteLine(result);
        }

        private string PrintEnd(Context context)
        {
            var stack = context.Tokens;
            ExpressionsHelper.CheckStack(context);
            var token = stack.Pop();
            if (token.Type == TokenType.QuoteString)
            {
                if (stack.Count > 0 && stack.Peek().TokenString == ",")
                {
                    stack.Pop();
                    return token.TokenString + " " + PrintEnd(context);
                }
                else
                    return token.TokenString;
            }
            else if (token.Type == TokenType.Character || token.Type == TokenType.Digit)
            {
                context.Tokens.Push(token);
                var expression = Expression.Parse(context).ToString();
                bool isNextComma = stack.Count > 0 && stack.Peek().TokenString == ",";
                if (stack.Count > 0 && isNextComma)
                {
                    stack.Pop();
                    return expression + " " + PrintEnd(context);
                }
                else
                    return expression;
            }
            else
                throw new Exception("Невалидное выражение <print_end>.");
        }

        private void For(Context context)
        {
            ExpressionsHelper.CheckStack(context);
            var stack = context.Tokens;
            var charecter = stack.Pop();
            if (charecter.Type != TokenType.Character)
                throw ExpressionsHelper.ThrowUnexpectedToken(charecter);
            ExpressionsHelper.CheckStack(context);
            var equal = stack.Pop();
            if (equal.Type != TokenType.Equal)
                throw ExpressionsHelper.ThrowUnexpectedToken(equal);
            var left = Expression.Parse(context);
            ExpressionsHelper.CheckStack(context);
            var to = stack.Pop();
            if (to.TokenString != "to")
                throw ExpressionsHelper.ThrowUnexpectedToken(equal);
            var right = Expression.Parse(context);
            context.Variables[charecter.TokenString] = left;
            ForLoop(context, charecter, right);
        }

        private void ForLoop(Context context, Token charecter, int right)
        {
            var queue = context.Tokens;
            var commands = new List<Token>();
            int bracketCount = 1;
            var token = queue.Pop();
            if (token.TokenString != "{")
                throw ExpressionsHelper.ThrowUnexpectedToken(token);
            commands.Add(token);
            while (queue.Count > 0 && bracketCount != 0)
            {
                token = queue.Pop();
                commands.Add(token);
                if (token.TokenString == "{")
                    bracketCount++;
                if (token.TokenString == "}")
                    bracketCount--;
            }
            if (bracketCount > 0)
                throw new Exception("не достаёт }.");

            while (context.Variables[charecter.TokenString] < right)
            {
                var forContext = new Context(commands);
                forContext.Variables = context.Variables;
                BracketStatement(forContext);
                context.Variables[charecter.TokenString]++;
            }
        }

        private void If(Context context)
        {
            var queue = context.Tokens;
            if (IfBool(context))
            {
                BracketStatement(context);
                if (queue.Peek().TokenString == "else")
                {
                    queue.Pop();
                    var token = queue.Pop();
                    int bracketCount = 1;
                    if (token.TokenString != "{")
                        throw ExpressionsHelper.ThrowUnexpectedToken(token);
                    while (queue.Count > 0 && bracketCount != 0)
                    {
                        token = queue.Pop();
                        if (token.TokenString == "{")
                            bracketCount++;
                        if (token.TokenString == "}")
                            bracketCount--;
                    }
                    if (bracketCount > 0)
                        throw new Exception("не достаёт }.");
                }
            }
            else
            {
                int bracketCount = 1;
                var token = queue.Pop();
                if (token.TokenString != "{")
                    throw ExpressionsHelper.ThrowUnexpectedToken(token);
                while (queue.Count > 0 && bracketCount != 0)
                {
                    token = queue.Pop();
                    if (token.TokenString == "{")
                        bracketCount++;
                    if (token.TokenString == "}")
                        bracketCount--;
                }
                if (bracketCount > 0)
                    throw new Exception("не достаёт }.");
                Else(context);
            }
        }

        private void BracketStatement(Context context)
        {
            var queue = context.Tokens;
            ExpressionsHelper.CheckStack(context);
            var token = queue.Pop();
            if (token.TokenString != "{")
                throw ExpressionsHelper.ThrowUnexpectedToken(token);
            while (queue.Count > 0 && queue.Peek().TokenString != "}")
                Statement(context);
            ExpressionsHelper.CheckStack(context);
            token = queue.Pop();
            if (token.TokenString != "}")
                throw ExpressionsHelper.ThrowUnexpectedToken(token);
        }

        private void Else(Context context)
        {
            var queue = context.Tokens;
            if (queue.Count == 0)
                return;
            var token = queue.Pop();
            if (token.TokenString == "else")
            {
                BracketStatement(context);
            }
        }

        private bool IfBool(Context context)
        {
            ExpressionsHelper.CheckStack(context);
            var left = Expression.Parse(context);
            ExpressionsHelper.CheckStack(context);
            var token = context.Tokens.Pop();
            var rigth = Expression.Parse(context);
            if (token.TokenString == ">")
            {
                return left > rigth;
            }
            else if (token.TokenString == "<")
            {
                return left < rigth;
            }
            else if (token.TokenString == "==")
            {
                return left == rigth;
            }
            else if (token.TokenString == "!=")
            {
                return left != rigth;
            }
            else
                throw ExpressionsHelper.ThrowUnexpectedToken(token);
        }

        public class Context
        {
            public Dictionary<string, int> Variables;
            public Stack<Token> Tokens;

            public Context(IEnumerable<Token> tokens)
            {
                Tokens = new Stack<Token>(tokens.Reverse());
                Variables = new Dictionary<string, int>();
            }
        }
    }
}
