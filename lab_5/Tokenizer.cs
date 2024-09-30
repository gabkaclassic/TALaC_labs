using System.Text.RegularExpressions;

namespace Interpreter
{
    public class Tokenizer
    {
        public IEnumerable<Token> Tokenize(string code)
        {
            var allRules = Rules.GetAllRules();
            var regexPattern = string.Join("|", allRules.Select(x =>
            {
                if ("()*+".Contains(x))
                    return $"({@"\" + x})";
                return $"({x})";
            }));
            var regex = Regex.Matches(code, regexPattern, RegexOptions.Singleline);

            foreach (Match item in regex)
            {
                var token = item.Value;

                if (string.IsNullOrWhiteSpace(token)) { yield return new Token(token, TokenType.Space); continue; }
                if (Rules.Functions.Contains(token)) { yield return new Token(token, TokenType.Function); continue; }
                if (Rules.Sintaxis.Contains(token)) { yield return new Token(token, TokenType.Sintaxis); continue; }
                if (Regex.Match(token, Rules.QuoteString).Success) { yield return new Token(token, TokenType.QuoteString); continue; }
                if (Regex.Match(token, Rules.Digit).Success) { yield return new Token(token, TokenType.Digit); continue; }
                if (Regex.Match(token, Rules.Character).Success) { yield return new Token(token, TokenType.Character); continue; }
                if (Rules.Operators.Contains(token)) { yield return new Token(token, TokenType.Operator); continue; }
                if (Rules.BoolOperations.Contains(token)) { yield return new Token(token, TokenType.BoolOperation); continue; }
                if (Rules.Equal == token) { yield return new Token(token, TokenType.Equal); continue; }
                if (Rules.Brackets.Contains(token)) { yield return new Token(token, TokenType.Bracket); continue; }
                if (Rules.Punctuation.Contains(token)) { yield return new Token(token, TokenType.Punctuation); continue; }
            }
        }

        public class Token
        {
            public Token(string token, TokenType identity)
            {
                TokenString = token;
                Type = identity;
            }

            public string TokenString { get; }
            public TokenType Type { get; }

            public override string ToString()
            {
                return $"{TokenString}|{Type}";
            }
        }

        public static class Rules
        {
            static Rules()
            {
                Digit = "\\d+";
                Character = "[a-zA-Z_]+";
                Operators = "+-*/".Select(x => x.ToString()).ToArray();
                BoolOperations = ["<", ">", "==", "!="];
                Equal = "=";
                Brackets = "(){}".Select(x => x.ToString()).ToArray();
                QuoteString = "\"(.*?)\"";
                Punctuation = ";,".Select(x => x.ToString()).ToArray();
                Space = "[\n\r\t ]+";
                Functions = ["scan", "print"];
                Sintaxis = ["for", "to", "if", "else"];
            }

            public static IEnumerable<string> GetAllRules()
            {
                var list = new List<string>();
                list.AddRange(Functions);
                list.AddRange(Sintaxis);
                list.Add(QuoteString);
                list.Add(Digit);
                list.Add(Character);
                list.AddRange(Operators);
                list.AddRange(BoolOperations);
                list.Add(Equal);
                list.AddRange(Brackets);
                list.AddRange(Punctuation);
                list.Add(Space);
                return list;
            }

            public static string Digit { get; }
            public static string Character { get; }
            public static string[] Operators { get; }
            public static string[] BoolOperations { get; }
            public static string Equal { get; }
            public static string[] Brackets { get; }
            public static string QuoteString { get; }
            public static string[] Punctuation { get; }
            public static string Space { get; }
            public static string[] Functions { get; }
            public static string[] Sintaxis { get; }
        }

        public enum TokenType
        {
            Digit,
            Character,
            Operator,
            BoolOperation,
            Equal,
            Bracket,
            Punctuation,
            QuoteString,
            Space,
            Function,
            Sintaxis
        }
    }
}
