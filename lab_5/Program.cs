namespace Interpreter
{
    public class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Файл: ");
            var file = Console.ReadLine();
            var code = File.ReadAllText($"Resources/{file}.txt");
            Interpreter interpreter = new Interpreter();
            interpreter.Interpret(code);
            Console.WriteLine("Программа завершена");
            Console.ReadLine();
        }
    }
}
