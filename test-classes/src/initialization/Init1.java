package initialization;

/**
 * Dynamic Dispatch During Instance Creation
 * Expect output: "0\n3\n"
 *
 * Status: Pass
 */
public class Init1 {
    private static class Super {
        Super() { printThree(); }
        void printThree() { System.out.println("three"); }
    }
    private static class Test extends Super {
        int three = (int)Math.PI;  // That is, 3
        void printThree() { System.out.println(three); }
    }

    public void test() {
        Test t = new Test();
        t.printThree();
    }
    
    public static void main(String[] args) {
        new Init1().test();
    }
    
}
