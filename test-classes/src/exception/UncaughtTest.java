package exception;

/**
 * Expect Output:
 * Exception in thread "main" java.lang.RuntimeException: BAD!
 *     at exception.UncaughtTest.bad(UncaughtTest.java:28)
 * 	   at exception.UncaughtTest.bar(UncaughtTest.java:24)
 * 	   at exception.UncaughtTest.foo(UncaughtTest.java:20)
 * 	   at exception.UncaughtTest.main(UncaughtTest.java:16)
 *
 * Status: Fail
 */
public class UncaughtTest {
    
    public static void main(String[] args) {
        foo();
    }
    
    private static void foo() {
        bar();
    }
    
    private static void bar() {
        bad();
    }
    
    private static void bad() {
        throw new RuntimeException("BAD!");
    }
    
}
