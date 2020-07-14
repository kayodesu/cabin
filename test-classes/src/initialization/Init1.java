package initialization;

/**
 * Dynamic Dispatch During Instance Creation
 * expect output: "0\n3\n"
 *
 * Status: Pass
 */
public class Eg12_5_2 {    
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
        new Eg12_5_2().test();
    }
    
}
