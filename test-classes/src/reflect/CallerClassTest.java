package reflect;

//import sun.reflect.Reflection;

/**
 * Status: Pass
 */
public class CallerClassTest {
    
    public static void main(String[] args) {
        Foo.test();
    }
    
    static class Foo {
        static void test() {
            Bar.test();
        }
    }
    
    static class Bar {
        static void test() {
//            System.out.println(Reflection.getCallerClass(0)); // sun.reflect.Reflection
//            System.out.println(Reflection.getCallerClass(1)); // reflect.CallerClassTest$Bar
//            System.out.println(Reflection.getCallerClass(2)); // reflect.CallerClassTest$Foo
//            System.out.println(Reflection.getCallerClass(3)); // reflect.CallerClassTest
//            System.out.println(Reflection.getCallerClass(4)); // null
        }
    }
}
