import lab_1.Calculator;
import lab_2.FiniteAutomaton;
import lab_2.State;
import lab_2.Transition;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;

public class Main {
    private static final Scanner scanner = new Scanner(System.in);
    public static void main(String[] args) throws Exception {

        lab_1();
    }

    private static void lab_1() {
        try {
            try {
                assert Calculator.evaluate("") == 0 : "Test failed: ''";
            } catch (Exception e) {
                assert true;
            }

            assert Calculator.evaluate("2") == 2 : "Test failed: 2";

            assert Calculator.evaluate("2 + 3 * 4") == 14 : "Test failed: 2 + 3 * 4";
            assert Calculator.evaluate("10 - 5") == 5 : "Test failed: 10 - 5";
            assert Calculator.evaluate("8 / 2") == 4 : "Test failed: 8 / 2";
            assert Calculator.evaluate("2 * 3") == 6 : "Test failed: 2 * 3";

            assert Calculator.evaluate("(2 + 3) * 4") == 20 : "Test failed: (2 + 3) * 4";
            assert Calculator.evaluate("2 * (3 + 4)") == 14 : "Test failed: 2 * (3 + 4)";

            assert Calculator.evaluate("2.5 + 3.5") == 6.0 : "Test failed: 2.5 + 3.5";
            assert Calculator.evaluate("1.5 * 2.5") == 3.75 : "Test failed: 1.5 * 2.5";

            assert Calculator.evaluate("-2.5 + 3.5") == 1.0 : "Test failed: -2.5 + 3.5";

            assert Calculator.evaluate("log(2, 1 + 2 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2") == 13.5 : "Test failed: log(2, 1 + 2 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2";
            assert Calculator.evaluate("log(2, 3 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2") == 13.084962500721156 : "Test failed: log(2, 3 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2";
            assert Calculator.evaluate("log(log(2, 120), 31)") == 1.7769482352082826 : "Test failed: log(2, 3 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2";
            assert Calculator.evaluate("log(log(2, 120), log(31, 2))") == -0.8280562836436048 : "Test failed: log(2, 3 * (2 - 1) * 0.5) + 5 * (2 + 3) / 2";
            try {
                Calculator.evaluate("log(1)");
                assert false : "Test failed: log(1) should throw an exception";
            } catch (Exception e) {
                assert true;
            }

            try {
                Calculator.evaluate("log(0, 1)");
                assert false : "Test failed: log(0, 1) should throw an exception";
            } catch (Exception e) {
                assert true;
            }
            try {
                Calculator.evaluate("a+b");
                assert false : "a+b failed";
            } catch (Exception e) {
                assert true;
            }
            try {
                Calculator.evaluate("a+s;ldfjlsnf;");
                assert false : "a+s;ldfjlsnf; failed";
            } catch (Exception e) {
                assert true;
            }

            try {
                Calculator.evaluate("2 + (3 * 4");
                assert false : "Test failed: 2 + (3 * 4 should throw an exception";
            } catch (Exception e) {
                assert true;
            }

            try {
                Calculator.evaluate("1 / 0");
                assert false : "Test failed: 1 / 0 should throw an exception";
            } catch (Exception e) {
                assert true;
            }

            System.out.println("All tests passed!");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
