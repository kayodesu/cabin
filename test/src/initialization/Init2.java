package initialization;

/**
 * Superclasses Are Initialized Before Subclasses
 *
 * Status: Pass
 */
public class Init2 {
    private static class Super {
        static { System.out.print("Super "); }
    }
    private static class One {
        static { System.out.print("One "); }
    }
    private static class Two extends Super {
        static { System.out.print("Two "); }
    }

    public void test() {
        One o = new One();
        Two t = new Two();
        System.out.println((Object)o == (Object)t);
    }
    
    public static void main(String[] args) {
        new Init2().test(); // print: One Super Two false
    }
    
}
