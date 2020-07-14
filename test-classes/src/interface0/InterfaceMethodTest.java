package interface0;

/**
 * Status: Fail
 */
public class InterfaceMethodTest {
    public interface If1 {
        static int x() {
            return 1;
        }
        default int y() {
            return 2;
        }
    }
    
    public static class Impl1 implements If1 {
        
    }
    
    public static class Impl2 implements If1 {        
        @Override
        public int y() {
            return 12;
        }
    }
    
    public static class Impl3 implements If1 {
        @Override
        public int y() {
            return 100 + If1.super.y();
        }
    }
    
    public static void main(String[] args) {
        InterfaceMethodTest t = new InterfaceMethodTest();
        t.defaultMethod();
        t.staticMethod();
    }

    public void staticMethod() {
        System.out.println(1 == If1.x() ? "Pass" : "Fail");
    }

    public void defaultMethod() {
        System.out.println(2 == new Impl1().y() ? "Pass" : "Fail");
        System.out.println(12 == new Impl2().y() ? "Pass" : "Fail");
        System.out.println(102 == new Impl3().y() ? "Pass" : "Fail");
    }
}
