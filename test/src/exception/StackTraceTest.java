package exception;

/**
 * Expect Output:
 * java.lang.RuntimeException: BAD!
 * 	   at exception.StackTraceTest.bad(StackTraceTest.java:18)
 * 	   at exception.StackTraceTest.bar(StackTraceTest.java:17)
 * 	   at exception.StackTraceTest.foo(StackTraceTest.java:16)
 * 	   at exception.StackTraceTest.main(StackTraceTest.java:10)
 *
 * Status: Pass
 */
public class StackTraceTest {
    
    public static void main(String[] args) {
        try {
            foo();
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }
    
    private static void foo() { bar(); }    
    private static void bar() { bad(); }    
    private static void bad() { throw new RuntimeException("BAD!"); }    
}
