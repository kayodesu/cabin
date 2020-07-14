package reflect;

/**
 * Status: Pass
 */
public class ClassTest1 {
    public static void main(String[] args) {
        Object obj = new Object();
        Class c1 = obj.getClass();
        Class c2 = c1.getClass();
        Class c3 = c2.getClass();
        System.out.println(c1); // class java.lang.Object
        System.out.println(c2); // class java.lang.Class
        System.out.println(c3); // class java.lang.Class
        System.out.println(c2 == c1); // false
        System.out.println(c2 == c3); // true
    }
}
