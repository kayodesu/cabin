package exception;

/**
 * Status: Pass
 */
public class EmptyCatchTest {
    
    public static void main(String[] args) {
        try {
            bad();
        } catch (Exception e) {
            // empty
        }
        
        System.out.println("Pass");
    }
    
    private static void bad() { throw new RuntimeException("BAD!"); }    
}
