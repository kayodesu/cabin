package initialization;

/**
 * Only The Class That Declares Static Field Is Initialized
 *
 * Status: Pass
 */
public class InitStaticField {
    
    private static class Super {
        static int taxi = 1729;
    }

    private static class Sub extends Super {
        static { 
            System.out.print("Sub ");
            if (true) {
                throw new RuntimeException("BAD");
            }
        }
    }

    public void test() {
        System.out.println(1729 == Sub.taxi ? "Pass": "Fail");
    }
    
    public static void main(String[] args) {
        new InitStaticField().test();
    }
}
