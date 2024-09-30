package lab_1;

import java.util.Stack;

public class Calculator {

    public static double evaluate(String expression) throws Exception {
        expression = expression.replaceAll("\\s+", "");
        if (!isValidExpression(expression)) {
            throw new Exception("Invalid expression");
        }
        return evaluateExpression(expression);
    }

    private static boolean isValidExpression(String expression) {
        if (expression == null || expression.isEmpty()) {
            return false;
        }

        Stack<Character> stack = new Stack<>();
        for (char c : expression.toCharArray()) {
            if (c == '(') {
                stack.push(c);
            } else if (c == ')') {
                if (stack.isEmpty() || stack.pop() != '(') {
                    return false;
                }
            }
        }
        return stack.isEmpty();
    }

    private static double evaluateExpression(String expression) throws Exception {
        StringBuilder rpn = new StringBuilder();
        return evaluate(expression, 0, expression.length() - 1, rpn);
    }
    private static double evaluate(String expression, int start, int end, StringBuilder rpn) throws Exception {
        Stack<Double> values = new Stack<>();
        Stack<Character> ops = new Stack<>();

        for (int i = start; i <= end; i++) {
            char c = expression.charAt(i);
            if (Character.isDigit(c) || c == '.') {
                int j = i;
                while (j <= end && (Character.isDigit(expression.charAt(j)) || expression.charAt(j) == '.')) {
                    j++;
                }
                double value = Double.parseDouble(expression.substring(i, j));
                values.push(value);
                i = j - 1;
            } else if (c == '(') {
                ops.push(c);
            } else if (c == ')') {
                while (ops.peek() != '(') {
                    char op = ops.pop();
                    double b = values.pop();
                    double a = values.pop();
                    double result = applyOp(op, b, a);
                    rpn.append(b).append(" ").append(a).append(" ").append(op).append(" ");
                    values.push(result);
                }
                ops.pop();
            } else if (c == '-' && (i == start || expression.charAt(i - 1) == '(' || expression.charAt(i - 1) == '+' || expression.charAt(i - 1) == '-' || expression.charAt(i - 1) == '*' || expression.charAt(i - 1) == '/')) {
                int j = i + 1;
                while (j <= end && (Character.isDigit(expression.charAt(j)) || expression.charAt(j) == '.')) {
                    j++;
                }
                double value = Double.parseDouble(expression.substring(i, j));
                values.push(value);
                rpn.append(value).append(" ");
                i = j - 1;
            } else if (c == '+' || c == '-' || c == '*' || c == '/') {
                while (!ops.isEmpty() && hasPrecedence(c, ops.peek())) {
                    char op = ops.pop();
                    double b = values.pop();
                    double a = values.pop();
                    double result = applyOp(op, b, a);
                    rpn.append(b).append(" ").append(a).append(" ").append(op).append(" ");
                    values.push(result);
                }
                ops.push(c);
            } else if (c == 'l') {
                int j = i + 3;
                int openBrackets = 0;
                int commaIndex = j;
                while (j <= end) {
                    if (expression.charAt(j) == '(') openBrackets++;
                    if (expression.charAt(j) == ')') openBrackets--;
                    if (openBrackets == 1 && expression.charAt(j) == ',') commaIndex = j;
                    if (openBrackets == 0) break;
                    j++;
                }
                try {
                    String func = expression.substring(i, j + 1);
                    double a = evaluate(func.substring(4, commaIndex).trim(), 0, commaIndex - 5, rpn);
                    double b = evaluate(func.substring(commaIndex + 1, func.length() - 1).trim(), 0, func.length() - commaIndex - 3, rpn);
                    double result = log(a, b);
                    values.push(result);
                    rpn.append(b).append(" ").append(a).append(" ").append("log").append(" ");

                    i = j;
                } catch (StringIndexOutOfBoundsException e) {
                    throw new IllegalArgumentException("Wrong log function syntax");
                }
            }
        }

        while (!ops.isEmpty()) {
            char op = ops.pop();
            double b = values.pop();
            double a = values.pop();
            double result = applyOp(op, b, a);

            values.push(result);
            rpn.append(b).append(" ").append(a).append(" ").append(op).append(" ");

        }

        double result = values.pop();
        System.out.println("RPN: " + rpn.toString().trim());
        return result;
    }


    private static boolean hasPrecedence(char op1, char op2) {
        if (op2 == '(' || op2 == ')') {
            return false;
        }
        return !((op1 == '*' || op1 == '/') && (op2 == '+' || op2 == '-'));
    }

    private static double applyOp(char op, double b, double a) throws Exception {
        return switch (op) {
            case '+' -> a + b;
            case '-' -> a - b;
            case '*' -> a * b;
            case '/' -> {
                if (b == 0) throw new Exception("Division by zero");
                yield a / b;
            }
            default -> 0;
        };
    }

    public static double log(double a, double b) {
        if (a <= 0 || b <= 0) throw new IllegalArgumentException("Logarithm base must be positive");
        return Math.log(b) / Math.log(a);
    }
}
